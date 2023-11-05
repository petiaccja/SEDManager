#include <Archive/Types/ValueToJSON.hpp>
#include <MockDevice/MockDevice.hpp>

#include <SEDManager/SEDManager.hpp>


static std::string lastErrorMessage;
static constexpr std::string_view mockDevicePath = "/dev/mock_device";
using namespace sedmgr;


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
const char* GetLastErrorMessage() {
    return AllocateString(lastErrorMessage);
}


char* EnumerateStorageDevices() {
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


std::shared_ptr<StorageDevice>* CreateStorageDevice(const char* path) {
    if (path == mockDevicePath) {
        const auto device = std::make_shared<MockDevice>();
        return new std::shared_ptr<StorageDevice>(device);
    }
    try {
        const auto device = std::make_shared<NvmeDevice>(path);
        const auto identity = device->IdentifyController(); // Test if device is actually NVMe.
        return new std::shared_ptr<StorageDevice>(device);
    }
    catch (...) {
        SetLastError(std::current_exception());
    }
    return nullptr;
}


char* StorageDevice_GetName(std::shared_ptr<StorageDevice>* device) {
    const auto info = (*device)->GetDesc();
    return AllocateString(info.name);
}


char* StorageDevice_GetSerial(std::shared_ptr<StorageDevice>* device) {
    const auto info = (*device)->GetDesc();
    return AllocateString(info.serial);
}


void ReleaseStorageDevice(std::shared_ptr<StorageDevice>* device) {
    delete device;
}


SEDManager* CreateSEDManager(std::shared_ptr<StorageDevice>* device) {
    try {
        return new SEDManager(*device);
    }
    catch (...) {
        SetLastError(std::current_exception());
    }
    return nullptr;
}


void ReleaseSEDManager(SEDManager* manager) {
    delete manager;
}


bool SEDManager_Start(SEDManager* manager, uint64_t sp) {
    try {
        manager->Start(sp);
        return true;
    }
    catch (std::exception&) {
        SetLastError(std::current_exception());
        return false;
    }
}


bool SEDManager_End(SEDManager* manager) {
    try {
        manager->End();
        return true;
    }
    catch (std::exception&) {
        SetLastError(std::current_exception());
        return false;
    }
}


uint64_t SEDManager_FindUID(SEDManager* manager, const char* name, uint64_t sp) {
    return manager->GetModules().FindUid(name, sp).value_or(0);
}


char* SEDManager_FindName(SEDManager* manager, uint64_t uid, uint64_t sp) {
    auto maybeName = manager->GetModules().FindName(uid, sp);
    if (maybeName) {
        return AllocateString(*maybeName);
    }
    return nullptr;
}


Table* CreateTable(SEDManager* manager, uint64_t uid) {
    try {
        return new Table(manager->GetTable(uid));
    }
    catch (std::exception&) {
        SetLastError(std::current_exception());
        return nullptr;
    }
}


void ReleaseTable(Table* table) {
    delete table;
}

struct TableIterator {
    Table::iterator first;
    Table::iterator last;
    bool initial = true;
};


TableIterator* CreateTableIterator(Table* table) {
    try {
        return new TableIterator(table->begin(), table->end());
    }
    catch (std::exception&) {
        SetLastError(std::current_exception());
        return nullptr;
    }
}


void ReleaseTableIterator(TableIterator* it) {
    delete it;
}


bool TableIterator_Next(TableIterator* it) {
    try {
        if (!it->initial && it->first != it->last) {
            std::advance(it->first, 1);
        }
        it->initial = false;
        return it->first != it->last;
    }
    catch (std::exception&) {
        SetLastError(std::current_exception());
        return false;
    }
}


Object* CreateObject(TableIterator* it) {
    try {
        return new Object(*it->first);
    }
    catch (std::exception&) {
        SetLastError(std::current_exception());
        return nullptr;
    }
}


void ReleaseObject(Object* object) {
    delete object;
}


uint64_t Object_GetUID(Object* object) {
    return object->Id();
}


char* Object_GetColumnNames(Object* object, size_t startColumn, size_t endColumn) {
    if (startColumn <= endColumn && endColumn <= object->GetDesc().size()) {
        std::string result;
        for (auto i = startColumn; i < endColumn; ++i) {
            const auto name = object->GetDesc()[i].name;
            result += name;
            result += '\0';
        }
        return AllocateString(result);
    }
    return nullptr;
}


char* Object_GetColumnValues(Object* object, size_t startColumn, size_t endColumn) {
    if (startColumn <= endColumn && endColumn <= object->GetDesc().size()) {
        std::string result;
        for (auto i = startColumn; i < endColumn; ++i) {
            try {
                const auto value = *(*object)[i];
                const auto type = object->GetDesc()[i].type;
                result += ValueToJSON(value, type).dump(2);
                result += '\0';
            }
            catch (std::exception& ex) {
                SetLastError(std::current_exception());
                return nullptr;
            }
        }
        return AllocateString(result);
    }
    return nullptr;
}


size_t Object_SetColumnValues(Object* object, size_t startColumn, size_t endColumn, const char* values) {
    size_t numSucceeded = 0;
    if (startColumn <= endColumn && endColumn <= object->GetDesc().size()) {
        for (auto i = startColumn; i < endColumn; ++i) {
            const auto json = std::string_view(values);
            const auto type = object->GetDesc()[i].type;
            try {
                const auto value = JSONToValue(nlohmann::json::parse(json), type);
                (*object)[i] = value;
                ++numSucceeded;
            }
            catch (std::exception& ex) {
                SetLastError(std::current_exception());
                return numSucceeded;
            }
        }
        return numSucceeded;
    }
    return numSucceeded;
}


void ReleaseString(const char* str) {
    delete[] str;
}
}