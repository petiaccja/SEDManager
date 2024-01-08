#include <Messaging/Value.hpp>
#include <MockDevice/MockDevice.hpp>

#include <EncryptedDevice/EncryptedDevice.hpp>


#ifdef _WIN32
    #define SEDMANAGER_EXPORT __declspec(dllexport)
#else
    #define SEDMANAGER_EXPORT __attribute__((visibility("default")))
#endif


using namespace sedmgr;


//------------------------------------------------------------------------------
// Error handling
//------------------------------------------------------------------------------


static std::exception_ptr lastException;


void SetLastException() {
    lastException = std::current_exception();
}


std::exception_ptr GetLastException() {
    return lastException;
}


//------------------------------------------------------------------------------
// Objects
//------------------------------------------------------------------------------


using CUID = uint64_t;


struct CString {
    std::string object;
};


struct CValue {
    Value object;
};


struct CStorageDevice {
    std::shared_ptr<StorageDevice> object;
};


struct CEncryptedDevice {
    CEncryptedDevice(EncryptedDevice object) : object(std::make_shared<EncryptedDevice>(std::move(object))) {}
    std::shared_ptr<EncryptedDevice> object;
};


template <class T>
struct CFuture {
    asyncpp::task<T> object;
};


template <class T>
struct CStream {
    asyncpp::stream<T> object;
};


using CFutureVoid = CFuture<void>;
using CFutureEncryptedDevice = CFuture<EncryptedDevice>;
using CFutureValue = CFuture<Value>;
using CFutureString = CFuture<std::string>;
using CFutureUID = CFuture<UID>;


using CStreamUID = CStream<UID>;
using CStreamString = CStream<std::string>;


//------------------------------------------------------------------------------
// Error handling
//------------------------------------------------------------------------------

extern "C"
{
    SEDMANAGER_EXPORT CString* CGetLastException_Message() {
        const auto lastException = GetLastException();
        if (lastException) {
            try {
                std::rethrow_exception(lastException);
            }
            catch (std::exception& ex) {
                return new CString{ ex.what() };
            }
        }
        return nullptr;
    }
}


//------------------------------------------------------------------------------
// CString
//------------------------------------------------------------------------------

extern "C"
{
    SEDMANAGER_EXPORT CString* CString_Create() {
        return new CString();
    }

    SEDMANAGER_EXPORT void CString_Destroy(CString* self) {
        delete self;
    }

    SEDMANAGER_EXPORT void CString_Set(CString* self, const char* value) {
        self->object = value;
    }

    SEDMANAGER_EXPORT const char* CString_Get(CString* self) {
        return self->object.data();
    }
}

//------------------------------------------------------------------------------
// CValue
//------------------------------------------------------------------------------

extern "C"
{
    SEDMANAGER_EXPORT CValue* CValue_Create() {
        return new CValue;
    }


    SEDMANAGER_EXPORT void CValue_Destroy(CValue* self) {
        delete self;
    }


    SEDMANAGER_EXPORT bool CValue_IsBytes(CValue* self) {
        return self->object.Is<Bytes>();
    }


    SEDMANAGER_EXPORT bool CValue_IsCommand(CValue* self) {
        return self->object.Is<eCommand>();
    }


    SEDMANAGER_EXPORT bool CValue_IsInteger(CValue* self) {
        return self->object.IsInteger();
    }


    SEDMANAGER_EXPORT bool CValue_IsList(CValue* self) {
        return self->object.Is<List>();
    }


    SEDMANAGER_EXPORT bool CValue_IsNamed(CValue* self) {
        return self->object.Is<Named>();
    }


    SEDMANAGER_EXPORT const std::byte* CValue_GetBytes(CValue* self) {
        if (!self->object.Is<Bytes>()) {
            return nullptr;
        }
        return self->object.Get<Bytes>().data();
    }


    SEDMANAGER_EXPORT uint8_t CValue_GetCommand(CValue* self) {
        if (!self->object.Is<eCommand>()) {
            return 0;
        }
        return static_cast<uint8_t>(self->object.Get<eCommand>());
    }


    SEDMANAGER_EXPORT int64_t CValue_GetInteger(CValue* self) {
        if (!self->object.IsInteger()) {
            return 0;
        }
        return self->object.Get<int64_t>();
    }


    SEDMANAGER_EXPORT CValue* CValue_GetList_Element(CValue* self, size_t element) {
        if (!self->object.Is<List>()) {
            return nullptr;
        }
        return new CValue{ self->object.Get<List>()[element] };
    }


    SEDMANAGER_EXPORT CValue* CValue_GetNamed_Name(CValue* self) {
        if (!self->object.Is<Named>()) {
            return nullptr;
        }
        return new CValue{ self->object.Get<Named>().name };
    }


    SEDMANAGER_EXPORT CValue* CValue_GetNamed_Value(CValue* self) {
        if (!self->object.Is<Named>()) {
            return nullptr;
        }
        return new CValue{ self->object.Get<Named>().value };
    }


    SEDMANAGER_EXPORT size_t CValue_GetLength(CValue* self) {
        if (self->object.Is<Bytes>()) {
            return self->object.Get<Bytes>().size();
        }
        if (self->object.Is<List>()) {
            return self->object.Get<List>().size();
        }
        return 1;
    }

    SEDMANAGER_EXPORT void CValue_SetBytes(CValue* self, const std::byte* value, size_t length) {
        self->object = std::span(value, length);
    }


    SEDMANAGER_EXPORT void CValue_SetCommand(CValue* self, uint8_t value) {
        self->object = static_cast<eCommand>(value);
    }


    SEDMANAGER_EXPORT void CValue_SetInteger(CValue* self, int64_t value, bool signedness, uint8_t width) {
        switch (width) {
            case 8: signedness ? self->object = int8_t(value) : self->object = uint8_t(value);
            case 16: signedness ? self->object = int16_t(value) : self->object = uint16_t(value);
            case 32: signedness ? self->object = int32_t(value) : self->object = uint32_t(value);
            default: signedness ? self->object = int64_t(value) : self->object = uint64_t(value);
        }
    }


    SEDMANAGER_EXPORT void CValue_SetList(CValue* self, const CValue** value, size_t length) {
        std::vector<Value> elements;
        elements.reserve(length);
        std::ranges::transform(std::span(value, length), std::back_inserter(elements), [](const CValue* v) {
            return v->object;
        });
        self->object = std::move(elements);
    }


    SEDMANAGER_EXPORT void CValue_SetNamed(CValue* self, const CValue* name, const CValue* value) {
        self->object = Named{ name->object, value->object };
    }
}


//------------------------------------------------------------------------------
// CStorageDevice
//------------------------------------------------------------------------------

static constexpr std::string_view mockDevicePath = "/dev/mock_device";


extern "C"
{
    SEDMANAGER_EXPORT CString* CEnumerateStorageDevices() {
        try {
            auto labels = EnumerateStorageDevices();

#ifndef NDEBUG
            labels.emplace_back(std::string(mockDevicePath), eStorageDeviceInterface::OTHER);
#endif

            std::string concated;
            for (auto& label : labels) {
                concated += label.path;
                concated += ';';
            }
            return new CString{ concated };
        }
        catch (std::exception&) {
            SetLastException();
            return nullptr;
        }
    }


    SEDMANAGER_EXPORT CStorageDevice* CStorageDevice_Create(CString* path) {
        try {
            if (path->object == mockDevicePath) {
                const auto device = std::make_shared<MockDevice>();
                return new CStorageDevice{ std::shared_ptr<StorageDevice>(device) };
            }
            const auto device = std::make_shared<NvmeDevice>(path->object);
            const auto identity = device->IdentifyController(); // Test if device is actually NVMe.
            return new CStorageDevice{ std::shared_ptr<StorageDevice>(device) };
        }
        catch (...) {
            SetLastException();
            return nullptr;
        }
    }


    SEDMANAGER_EXPORT CString* CStorageDevice_GetName(CStorageDevice* self) {
        const auto info = self->object->GetDesc();
        return new CString{ info.name };
    }


    SEDMANAGER_EXPORT CString* CStorageDevice_GetSerial(CStorageDevice* self) {
        const auto info = self->object->GetDesc();
        return new CString{ info.serial };
    }


    SEDMANAGER_EXPORT void CStorageDevice_Destroy(CStorageDevice* self) {
        delete self;
    }
}


//------------------------------------------------------------------------------
// CEncryptedDevice
//------------------------------------------------------------------------------


extern "C"
{
    SEDMANAGER_EXPORT CFutureEncryptedDevice* CEncryptedDevice_Create(CStorageDevice* storageDevice) {
        return new CFutureEncryptedDevice{ EncryptedDevice::Start(storageDevice->object) };
    }


    SEDMANAGER_EXPORT void CEncryptedDevice_Destroy(CEncryptedDevice* self) {
        delete self;
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Login(CEncryptedDevice* self, CUID securityProvider) {
        return new CFutureVoid{ self->object->Login(UID(securityProvider)) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Authenticate(CEncryptedDevice* self, CUID authority, CString* password) {
        const auto _password = password ? std::optional(as_bytes(std::span(password->object))) : std::nullopt;
        return new CFutureVoid{ self->object->Authenticate(UID(authority), _password) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_StackReset(CEncryptedDevice* self) {
        return new CFutureVoid{ self->object->StackReset() };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Reset(CEncryptedDevice* self) {
        return new CFutureVoid{ self->object->Reset() };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_End(CEncryptedDevice* self) {
        return new CFutureVoid{ self->object->End() };
    }


    SEDMANAGER_EXPORT CFutureString* CEncryptedDevice_FindName(CEncryptedDevice* self,
                                                               CUID uid,
                                                               CUID securityProvider) {
        const auto& modules = self->object->GetModules();
        const auto maybeSp = securityProvider != 0 ? std::optional(UID(securityProvider)) : std::nullopt;
        const auto maybeName = modules.FindName(UID(uid), maybeSp);
        return new CFutureString{ [maybeName]() -> asyncpp::task<std::string> {
            if (!maybeName) {
                throw std::invalid_argument("could not find table description");
            }
            co_return *maybeName;
        }() };
    }


    SEDMANAGER_EXPORT CFutureUID* CEncryptedDevice_FindUID(CEncryptedDevice* self,
                                                           CString* name,
                                                           CUID securityProvider) {
        const auto& modules = self->object->GetModules();
        const auto maybeSp = securityProvider != 0 ? std::optional(UID(securityProvider)) : std::nullopt;
        const auto maybeUID = modules.FindUid(name->object, maybeSp);
        return new CFutureUID{ [maybeUID]() -> asyncpp::task<UID> {
            if (!maybeUID) {
                throw std::invalid_argument("could not find table description");
            }
            co_return *maybeUID;
        }() };
    }


    SEDMANAGER_EXPORT CStreamUID* CEncryptedDevice_GetTableRows(CEncryptedDevice* self, CUID table) {
        return new CStreamUID{ self->object->GetTableRows(UID(table)) };
    }


    SEDMANAGER_EXPORT CStreamString* CEncryptedDevice_GetTableColumns(CEncryptedDevice* self, CUID table) {
        const auto& modules = self->object->GetModules();
        const auto maybeDesc = modules.FindTable(UID(table));
        return new CStreamString{ [maybeDesc]() -> asyncpp::stream<std::string> {
            if (!maybeDesc) {
                throw std::invalid_argument("could not find table description");
            }
            for (const auto& column : maybeDesc->columns) {
                co_yield column.name;
            }
        }() };
    }


    SEDMANAGER_EXPORT CFutureValue* CEncryptedDevice_GetObjectColumn(CEncryptedDevice* self, CUID object, uint32_t column) {
        return new CFutureValue{ self->object->GetObjectColumn(UID(object), column) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_SetObjectColumn(CEncryptedDevice* self, CUID object, uint32_t column, CValue* value) {
        return new CFutureVoid{ self->object->SetObjectColumn(UID(object), column, value->object) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_GenMEK(CEncryptedDevice* self, CUID lockingRange) {
        return new CFutureVoid{ self->object->GenMEK(UID(lockingRange)) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_GenPIN(CEncryptedDevice* self, CUID credentialObject, uint32_t length) {
        return new CFutureVoid{ self->object->GenPIN(UID(credentialObject), length) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Revert(CEncryptedDevice* self, CUID securityProvider) {
        return new CFutureVoid{ self->object->Revert(UID(securityProvider)) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Activate(CEncryptedDevice* self, CUID securityProvider) {
        return new CFutureVoid{ self->object->Activate(UID(securityProvider)) };
    }
}

//------------------------------------------------------------------------------
// Futures
//------------------------------------------------------------------------------

template <class Ty>
void CFuture_Destroy(CFuture<Ty>* self) {
    delete self;
}


template <class Ty, class Wrapper>
void CFuture_Start(CFuture<Ty>* self, void (*callback)(bool, Wrapper)) {
    launch([](auto task, auto callback) -> asyncpp::task<void> {
        try {
            if constexpr (std::is_void_v<Ty>) {
                co_await task;
                callback(true, nullptr);
            }
            else {
                auto result = co_await task;
                using WrapperBase = std::remove_pointer_t<Wrapper>;
                if constexpr (std::is_pointer_v<Wrapper> && !std::is_pointer_v<Ty>) {
                    callback(true, new WrapperBase(std::move(result)));
                }
                else {
                    callback(true, WrapperBase(std::move(result)));
                }
            }
        }
        catch (...) {
            SetLastException();
            callback(false, Wrapper{});
        }
    }(std::move(self->object), callback));
}


template <class Ty>
void CStream_Destroy(CStream<Ty>* self) {
    delete self;
}


template <class Ty, class Wrapper>
void CStream_Start(CStream<Ty>* self, void (*callback)(bool, bool, Wrapper)) {
    launch([](auto stream, auto callback) -> asyncpp::task<void> {
        asyncpp::await_result_t<std::decay_t<decltype(stream)>> result;
        do {
            result = co_await stream;
            if (result) {
                try {
                    using WrapperBase = std::remove_pointer_t<Wrapper>;
                    if constexpr (std::is_pointer_v<Wrapper> && !std::is_pointer_v<Ty>) {
                        callback(true, true, new WrapperBase(std::move(*result)));
                    }
                    else {
                        callback(true, true, WrapperBase(std::move(*result)));
                    }
                }
                catch (...) {
                    SetLastException();
                    callback(true, false, Wrapper{});
                }
            }
        } while (result);
        callback(false, false, Wrapper{});
    }(std::move(self->object), callback));
}


extern "C"
{
    SEDMANAGER_EXPORT void CFutureVoid_Destroy(CFutureVoid* self) {
        CFuture_Destroy(self);
    }


    SEDMANAGER_EXPORT void CFutureVoid_Start(CFutureVoid* self, void (*callback)(bool, void*)) {
        CFuture_Start(self, callback);
    }


    SEDMANAGER_EXPORT void CFutureString_Destroy(CFutureString* self) {
        CFuture_Destroy(self);
    }


    SEDMANAGER_EXPORT void CFutureString_Start(CFutureString* self, void (*callback)(bool, CString*)) {
        CFuture_Start(self, callback);
    }


    SEDMANAGER_EXPORT void CFutureValue_Destroy(CFutureValue* self) {
        CFuture_Destroy(self);
    }


    SEDMANAGER_EXPORT void CFutureValue_Start(CFutureValue* self, void (*callback)(bool, CValue*)) {
        CFuture_Start(self, callback);
    }


    SEDMANAGER_EXPORT void CFutureUID_Destroy(CFutureUID* self) {
        CFuture_Destroy(self);
    }


    SEDMANAGER_EXPORT void CFutureUID_Start(CFutureUID* self, void (*callback)(bool, CUID)) {
        CFuture_Start(self, callback);
    }


    SEDMANAGER_EXPORT void CFutureEncryptedDevice_Destroy(CFutureEncryptedDevice* self) {
        CFuture_Destroy(self);
    }


    SEDMANAGER_EXPORT void CFutureEncryptedDevice_Start(CFutureEncryptedDevice* self, void (*callback)(bool, CEncryptedDevice*)) {
        CFuture_Start(self, callback);
    }


    SEDMANAGER_EXPORT void CStreamUID_Destroy(CStreamUID* self) {
        CStream_Destroy(self);
    }


    SEDMANAGER_EXPORT void CStreamUID_Start(CStreamUID* self, void (*callback)(bool, bool, CUID)) {
        CStream_Start(self, callback);
    }


    SEDMANAGER_EXPORT void CStreamString_Destroy(CStreamString* self) {
        CStream_Destroy(self);
    }


    SEDMANAGER_EXPORT void CStreamString_Start(CStreamString* self, void (*callback)(bool, bool, CString*)) {
        CStream_Start(self, callback);
    }
}