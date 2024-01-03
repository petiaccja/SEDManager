#include <Data/Value.hpp>
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


using CUid = uint64_t;


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
    std::shared_ptr<EncryptedDevice> object;
};


struct CFutureVoid {
    asyncpp::task<void> object;
};


struct CFutureEncryptedDevice {
    asyncpp::task<EncryptedDevice> object;
};


struct CFutureValue {
    asyncpp::task<Value> object;
};


struct CStreamUid {
    asyncpp::stream<Uid> object;
};


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
        return self->object.IsBytes();
    }


    SEDMANAGER_EXPORT bool CValue_IsCommand(CValue* self) {
        return self->object.IsCommand();
    }


    SEDMANAGER_EXPORT bool CValue_IsInteger(CValue* self) {
        return self->object.IsInteger();
    }


    SEDMANAGER_EXPORT bool CValue_IsList(CValue* self) {
        return self->object.IsList();
    }


    SEDMANAGER_EXPORT bool CValue_IsNamed(CValue* self) {
        return self->object.IsNamed();
    }


    SEDMANAGER_EXPORT const std::byte* CValue_GetBytes(CValue* self) {
        if (!self->object.IsBytes()) {
            return nullptr;
        }
        return self->object.GetBytes().data();
    }


    SEDMANAGER_EXPORT uint8_t CValue_GetCommand(CValue* self) {
        if (!self->object.IsCommand()) {
            return 0;
        }
        return static_cast<uint8_t>(self->object.GetCommand());
    }


    SEDMANAGER_EXPORT int64_t CValue_GetInteger(CValue* self) {
        if (!self->object.IsInteger()) {
            return 0;
        }
        return self->object.GetInt<int64_t>();
    }


    SEDMANAGER_EXPORT CValue* CValue_GetList_Element(CValue* self, size_t element) {
        if (!self->object.IsList()) {
            return nullptr;
        }
        return new CValue{ self->object.GetList()[element] };
    }


    SEDMANAGER_EXPORT CValue* CValue_GetNamed_Name(CValue* self) {
        if (!self->object.IsNamed()) {
            return nullptr;
        }
        return new CValue{ self->object.GetNamed().name };
    }


    SEDMANAGER_EXPORT CValue* CValue_GetNamed_Value(CValue* self) {
        if (!self->object.IsNamed()) {
            return nullptr;
        }
        return new CValue{ self->object.GetNamed().value };
    }


    SEDMANAGER_EXPORT size_t CValue_GetLength(CValue* self) {
        if (self->object.IsBytes()) {
            return self->object.GetBytes().size();
        }
        if (self->object.IsList()) {
            return self->object.GetList().size();
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


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Login(CEncryptedDevice* self, CUid securityProvider) {
        return new CFutureVoid{ self->object->Login(securityProvider) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Authenticate(CEncryptedDevice* self, CUid authority, CString* password) {
        const auto _password = password ? std::optional(as_bytes(std::span(password->object))) : std::nullopt;
        return new CFutureVoid{ self->object->Authenticate(authority, _password) };
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


    SEDMANAGER_EXPORT CStreamUid* CEncryptedDevice_GetTableRows(CEncryptedDevice* self, CUid table) {
        return new CStreamUid{ self->object->GetTableRows(table) };
    }


    SEDMANAGER_EXPORT CFutureValue* CEncryptedDevice_GetObjectColumn(CEncryptedDevice* self, CUid object, uint32_t column) {
        return new CFutureValue{ self->object->GetObjectColumn(object, column) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_SetObjectColumn(CEncryptedDevice* self, CUid object, uint32_t column, CValue* value) {
        return new CFutureVoid{ self->object->SetObjectColumn(object, column, value->object) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_GenMEK(CEncryptedDevice* self, CUid lockingRange) {
        return new CFutureVoid{ self->object->GenMEK(lockingRange) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_GenPIN(CEncryptedDevice* self, CUid credentialObject, uint32_t length) {
        return new CFutureVoid{ self->object->GenPIN(credentialObject, length) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Revert(CEncryptedDevice* self, CUid securityProvider) {
        return new CFutureVoid{ self->object->Revert(securityProvider) };
    }


    SEDMANAGER_EXPORT CFutureVoid* CEncryptedDevice_Activate(CEncryptedDevice* self, CUid securityProvider) {
        return new CFutureVoid{ self->object->Activate(securityProvider) };
    }
}

//------------------------------------------------------------------------------
// Futures
//------------------------------------------------------------------------------

extern "C"
{
    SEDMANAGER_EXPORT void CFutureVoid_Destroy(CFutureVoid* self) {
        delete self;
    }


    SEDMANAGER_EXPORT void CFutureVoid_Start(CFutureVoid* self, void (*callback)(void*)) {
        launch([](auto task, auto callback) -> asyncpp::task<void> {
            try {
                co_await task;
                callback(reinterpret_cast<void*>(1));
            }
            catch (...) {
                SetLastException();
                callback(nullptr);
            }
        }(std::move(self->object), callback));
    }


    SEDMANAGER_EXPORT void CFutureEncryptedDevice_Destroy(CFutureEncryptedDevice* self) {
        delete self;
    }


    SEDMANAGER_EXPORT void CFutureEncryptedDevice_Start(CFutureEncryptedDevice* self, void (*callback)(CEncryptedDevice*)) {
        launch([](auto task, auto callback) -> asyncpp::task<void> {
            try {
                auto device = co_await task;
                callback(new CEncryptedDevice{ std::make_shared<EncryptedDevice>(std::move(device)) });
            }
            catch (...) {
                SetLastException();
                callback(nullptr);
            }
        }(std::move(self->object), callback));
    }


    SEDMANAGER_EXPORT void CFutureValue_Destroy(CFutureValue* self) {
        delete self;
    }


    SEDMANAGER_EXPORT void CFutureValue_Start(CFutureValue* self, void (*callback)(CValue*)) {
        launch([](auto task, auto callback) -> asyncpp::task<void> {
            try {
                auto result = co_await task;
                callback(new CValue{ std::move(result) });
            }
            catch (...) {
                SetLastException();
                callback(nullptr);
            }
        }(std::move(self->object), callback));
    }


    SEDMANAGER_EXPORT void CStreamUid_Destroy(CStreamUid* self) {
        delete self;
    }


    SEDMANAGER_EXPORT void CStreamUid_Start(CStreamUid* self, void (*callback)(bool, CUid)) {
        launch([](auto stream, auto callback) -> asyncpp::task<void> {
            std::optional<Uid> result;
            do {
                try {
                    result = co_await stream;
                    if (result) {
                        callback(true, *result);
                    }
                    else {
                        callback(false, 0);
                    }
                }
                catch (...) {
                    SetLastException();
                    callback(true, 0);
                }
            } while (result);
        }(std::move(self->object), callback));
    }
}