#include <Archive/Types/ValueToJSON.hpp>
#include <MockDevice/MockDevice.hpp>

#include <EncryptedDevice/EncryptedDevice.hpp>


#ifdef _WIN32
    #define SEDMANAGER_EXPORT __declspec(dllexport)
#else
    #define SEDMANAGER_EXPORT __attribute__((visibility("default")))
#endif


static std::string lastErrorMessage;
static constexpr std::string_view mockDevicePath = "/dev/mock_device";
using namespace sedmgr;


enum class eStatusCode : uint32_t {
    SUCCESS = 0,
    FAIL = 1,
};


void SetLastError(std::exception_ptr ex) {
    try {
        std::rethrow_exception(ex);
    }
    catch (std::exception& ex) {
        lastErrorMessage = ex.what();
    }
    catch (...) {
        lastErrorMessage = "unrecognized exception was thrown -- this is bad and should not happen";
    }
}


char* AllocateString(std::string_view str) {
    char* buffer = new char[str.size() + 1];
    std::ranges::copy(str, buffer);
    buffer[str.size()] = '\0';
    return buffer;
}


extern "C"
{
    struct StorageDevice_CAPI {
        std::shared_ptr<StorageDevice> handle;
    };


    struct EncryptedDevice_CAPI {
        std::shared_ptr<EncryptedDevice> handle;
    };


    SEDMANAGER_EXPORT const char* GetLastErrorMessage() {
        return AllocateString(lastErrorMessage);
    }

    SEDMANAGER_EXPORT char* EnumerateStorageDevices() {
        try {
            auto labels = sedmgr::EnumerateStorageDevices();

#ifndef NDEBUG
            labels.emplace_back(std::string(mockDevicePath), eStorageDeviceInterface::OTHER);
#endif

            std::string concated;
            for (auto& label : labels) {
                concated += label.path;
                concated += ';';
            }
            return AllocateString(concated);
        }
        catch (std::exception&) {
            SetLastError(std::current_exception());
            return nullptr;
        }
    }


    SEDMANAGER_EXPORT StorageDevice_CAPI* StorageDevice_Create(const char* path) {
        if (path == mockDevicePath) {
            const auto device = std::make_shared<MockDevice>();
            return new StorageDevice_CAPI{ std::shared_ptr<StorageDevice>(device) };
        }
        try {
            const auto device = std::make_shared<NvmeDevice>(path);
            const auto identity = device->IdentifyController(); // Test if device is actually NVMe.
            return new StorageDevice_CAPI{ std::shared_ptr<StorageDevice>(device) };
        }
        catch (...) {
            SetLastError(std::current_exception());
        }
        return nullptr;
    }


    SEDMANAGER_EXPORT char* StorageDevice_GetName(StorageDevice_CAPI* device) {
        const auto info = device->handle->GetDesc();
        return AllocateString(info.name);
    }


    SEDMANAGER_EXPORT char* StorageDevice_GetSerial(StorageDevice_CAPI* device) {
        const auto info = device->handle->GetDesc();
        return AllocateString(info.serial);
    }


    SEDMANAGER_EXPORT void StorageDevice_Release(StorageDevice_CAPI* device) {
        delete device;
    }


    SEDMANAGER_EXPORT EncryptedDevice_CAPI* EncryptedDevice_Create(StorageDevice_CAPI* device) {
        try {
            return new EncryptedDevice_CAPI{ std::make_shared<EncryptedDevice>(device->handle) };
        }
        catch (...) {
            SetLastError(std::current_exception());
        }
        return nullptr;
    }


    SEDMANAGER_EXPORT void EncryptedDevice_Start(StorageDevice_CAPI* device,
                                                 void (*callback)(eStatusCode, EncryptedDevice_CAPI*)) {
        auto wrapper = [=] { return EncryptedDevice::Start(device->handle); };
        launch([](auto wrapper, auto callback) -> asyncpp::task<void> {
            try {
                const auto encryptedDevice = std::make_shared<EncryptedDevice>(co_await wrapper());
                callback(eStatusCode::SUCCESS, new EncryptedDevice_CAPI{ encryptedDevice });
            }
            catch (std::exception&) {
                SetLastError(std::current_exception());
                callback(eStatusCode::FAIL, nullptr);
            }
        }(std::move(wrapper), callback));
    }


    SEDMANAGER_EXPORT void EncryptedDevice_Login(EncryptedDevice_CAPI* device,
                                                 void (*callback)(eStatusCode),
                                                 uint64_t securityProvider) {
        auto wrapper = [=] { return device->handle->Login(securityProvider); };
        launch([](auto wrapper, auto callback) -> asyncpp::task<void> {
            try {
                co_await wrapper();
                callback(eStatusCode::SUCCESS);
            }
            catch (std::exception&) {
                SetLastError(std::current_exception());
                callback(eStatusCode::FAIL);
            }
        }(std::move(wrapper), callback));
    }


    SEDMANAGER_EXPORT void EncryptedDevice_End(EncryptedDevice_CAPI* device,
                                               void (*callback)(eStatusCode)) {
        auto wrapper = [=] { return device->handle->End(); };
        launch([](auto wrapper, auto callback) -> asyncpp::task<void> {
            try {
                co_await wrapper();
                callback(eStatusCode::SUCCESS);
            }
            catch (std::exception&) {
                SetLastError(std::current_exception());
                callback(eStatusCode::FAIL);
            }
        }(std::move(wrapper), callback));
    }


    SEDMANAGER_EXPORT void EncryptedDevice_FindName(EncryptedDevice_CAPI* device,
                                                    void (*callback)(eStatusCode, const char*),
                                                    uint64_t uid,
                                                    uint64_t securityProvider) {
        const auto& modules = device->handle->GetModules();
        const auto maybeName = modules.FindName(uid, securityProvider != 0 ? std::optional(securityProvider) : std::nullopt);
        if (maybeName) {
            callback(eStatusCode::SUCCESS, AllocateString(*maybeName));
        }
        else {
            callback(eStatusCode::FAIL, nullptr);
        }
    }


    SEDMANAGER_EXPORT void EncryptedDevice_FindUid(EncryptedDevice_CAPI* device,
                                                   void (*callback)(eStatusCode, uint64_t),
                                                   const char* name,
                                                   uint64_t securityProvider) {
        const auto& modules = device->handle->GetModules();
        const auto maybeUid = modules.FindUid(name, securityProvider != 0 ? std::optional(securityProvider) : std::nullopt);
        if (maybeUid) {
            callback(eStatusCode::SUCCESS, *maybeUid);
        }
        else {
            callback(eStatusCode::FAIL, 0);
        }
    }


    SEDMANAGER_EXPORT void EncryptedDevice_GetTableRows(EncryptedDevice_CAPI* device,
                                                        void (*callback)(eStatusCode, uint64_t),
                                                        uint64_t tableUid) {
        auto wrapper = [=] { return device->handle->GetTableRows(tableUid); };
        launch([](auto wrapper, auto callback) -> asyncpp::task<void> {
            try {
                auto stream = wrapper();
                while (const auto maybeRowUid = co_await stream) {
                    callback(eStatusCode::SUCCESS, *maybeRowUid);
                }
                callback(eStatusCode::SUCCESS, 0);
            }
            catch (std::exception&) {
                SetLastError(std::current_exception());
                callback(eStatusCode::FAIL, 0);
            }
        }(std::move(wrapper), callback));
    }


    SEDMANAGER_EXPORT void EncryptedDevice_GetTableColumns(EncryptedDevice_CAPI* device,
                                                           void (*callback)(eStatusCode, const char*),
                                                           uint64_t tableUid) {
        const auto& modules = device->handle->GetModules();
        const auto maybeDesc = modules.FindTable(tableUid);
        if (maybeDesc) {
            for (const auto& column : maybeDesc->columns) {
                callback(eStatusCode::SUCCESS, AllocateString(column.name));
            }
            callback(eStatusCode::SUCCESS, nullptr);
        }
        else {
            lastErrorMessage = "could not find table description";
            callback(eStatusCode::FAIL, nullptr);
        }
    }


    SEDMANAGER_EXPORT void EncryptedDevice_GetObjectColumn(EncryptedDevice_CAPI* device,
                                                           void (*callback)(eStatusCode, const char*),
                                                           uint64_t securityProviderUid,
                                                           uint64_t tableUid,
                                                           uint64_t objectUid,
                                                           uint32_t column) {
        auto wrapper = [=]() -> asyncpp::task<std::string> {
            const auto& modules = device->handle->GetModules();
            const auto maybeSpUid = securityProviderUid != 0 ? std::optional(securityProviderUid) : std::nullopt;
            const auto maybeDesc = modules.FindTable(tableUid);
            if (!maybeDesc) {
                throw std::invalid_argument("could not find table description");
            }
            if (column >= maybeDesc->columns.size()) {
                throw std::invalid_argument("column out of range");
            }
            const auto& columnDesc = maybeDesc->columns[column];
            const auto nameConverter = [&](Uid uid) { return modules.FindName(uid, maybeSpUid); };
            const auto value = co_await device->handle->GetObjectColumn(objectUid, column);
            if (value.HasValue()) {
                co_return ValueToJSON(value, columnDesc.type, nameConverter).dump();
            }
            co_return "";
        };
        launch([](auto wrapper, auto callback) -> asyncpp::task<void> {
            try {
                const std::string value = co_await wrapper();
                callback(eStatusCode::SUCCESS, AllocateString(value));
            }
            catch (std::exception&) {
                SetLastError(std::current_exception());
                callback(eStatusCode::FAIL, nullptr);
            }
        }(std::move(wrapper), callback));
    }


    SEDMANAGER_EXPORT void EncryptedDevice_SetObjectColumn(EncryptedDevice_CAPI* device,
                                                           void (*callback)(eStatusCode),
                                                           uint64_t securityProviderUid,
                                                           uint64_t tableUid,
                                                           uint64_t objectUid,
                                                           uint32_t column,
                                                           const char* jsonValue) {
        auto wrapper = [=]() -> asyncpp::task<void> {
            const auto& modules = device->handle->GetModules();
            const auto maybeSpUid = securityProviderUid != 0 ? std::optional(securityProviderUid) : std::nullopt;
            const auto maybeDesc = modules.FindTable(tableUid);
            if (!maybeDesc) {
                throw std::invalid_argument("could not find table description");
            }
            if (column >= maybeDesc->columns.size()) {
                throw std::invalid_argument("column out of range");
            }
            const auto& columnDesc = maybeDesc->columns[column];
            const auto nameConverter = [&](std::string_view name) { return modules.FindUid(name, maybeSpUid); };
            const auto value = JSONToValue(nlohmann::json::parse(jsonValue), columnDesc.type, nameConverter);
            co_await device->handle->SetObjectColumn(objectUid, column, value);
        };
        launch([](auto wrapper, auto callback) -> asyncpp::task<void> {
            try {
                co_await wrapper();
                callback(eStatusCode::SUCCESS);
            }
            catch (std::exception&) {
                SetLastError(std::current_exception());
                callback(eStatusCode::FAIL);
            }
        }(std::move(wrapper), callback));
    }


    SEDMANAGER_EXPORT void EncryptedDevice_Release(EncryptedDevice_CAPI* device) {
        delete device;
    }


    SEDMANAGER_EXPORT void String_Release(const char* str) {
        delete[] str;
    }
}