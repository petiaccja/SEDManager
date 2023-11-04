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


Table* CreateTable(SEDManager* manager, uint64_t uid) {
    return new Table(manager->GetTable(uid));
}


void ReleaseTable(Table* table) {
    delete table;
}


using TableIterator = std::pair<Table::iterator, Table::iterator>;


TableIterator* CreateObject(Table* table) {
    return new TableIterator(table->begin(), table->end());
}


void ReleaseObject(TableIterator* object) {
    delete object;
}


void Object_Next(TableIterator* object) {
    std::advance(object->first, 1);
}


bool Object_Valid(TableIterator* object) {
    return object->first != object->second;
}


char* ObjectGet_ColumnNames(TableIterator* object, size_t startColumn, size_t endColumn) {
    Object obj = *object->first;
    if (startColumn <= endColumn && endColumn <= obj.GetDesc().size()) {
        std::string result;
        for (auto i = startColumn; i < endColumn; ++i) {
            const auto name = obj.GetDesc()[i].name;
            result += name;
        }
        char* buffer = new char[result.size() + 1];
        std::ranges::copy(result, buffer);
        buffer[result.size()] = '\0';
        return buffer;
    }
    return nullptr;
}


char* ObjectGet_ColumnValues(TableIterator* object, size_t startColumn, size_t endColumn) {
    Object obj = *object->first;
    if (startColumn <= endColumn && endColumn <= obj.GetDesc().size()) {
        std::string result;
        for (auto i = startColumn; i < endColumn; ++i) {
            const auto value = *obj[i];
            const auto type = obj.GetDesc()[i].type;
            try {
                result += ValueToJSON(value, type).dump(2);
            }
            catch (std::exception& ex) {
                SetLastError(std::current_exception());
                return nullptr;
            }
        }
        char* buffer = new char[result.size() + 1];
        std::ranges::copy(result, buffer);
        buffer[result.size()] = '\0';
        return buffer;
    }
    return nullptr;
}


size_t Object_SetColumnValues(TableIterator* object, size_t startColumn, size_t endColumn, const char* values) {
    Object obj = *object->first;
    size_t numSucceeded = 0;
    if (startColumn <= endColumn && endColumn <= obj.GetDesc().size()) {
        for (auto i = startColumn; i < endColumn; ++i) {
            const auto json = std::string_view(values);
            const auto type = obj.GetDesc()[i].type;
            try {
                const auto value = JSONToValue(nlohmann::json::parse(json), type);
                obj[i] = value;
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