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


template <class Iter>
size_t CopyString(Iter first, Iter last, char* output, size_t outputSize) {
    const auto outputSizeNull = ptrdiff_t(outputSize) - 1;
    ptrdiff_t outputIndex = 0;
    for (; first != last && outputIndex < outputSizeNull; ++first, ++outputIndex) {
        output[outputIndex] = *first;
    }
    if (outputSize > 0) {
        output[outputIndex] = '\0';
        ++outputIndex;
    }
    return size_t(outputIndex);
}


extern "C"
{
const char* GetLastErrorMessage() {
    return lastErrorMessage.data();
}


size_t EnumerateStorageDevices(char* const devices, const size_t length) {
    auto labels = sedmgr::EnumerateStorageDevices();

#ifndef NDEBUG
    labels.emplace_back(std::string(mockDevicePath), eStorageDeviceInterface::OTHER);
#endif

    size_t writeIndex = 0;
    ptrdiff_t numRemaining = devices != nullptr ? ptrdiff_t(length) : std::numeric_limits<ptrdiff_t>::max();
    for (auto& label : labels) {
        if (devices != nullptr && numRemaining >= 2) {
            const auto copied = CopyString(label.path.begin(), label.path.end(), devices + writeIndex, numRemaining - 1);
            writeIndex += copied;
            numRemaining -= copied;
        }
        else {
            writeIndex += label.path.size() + 1;
        }
    }
    if (devices && numRemaining >= 1) {
        devices[writeIndex] = '\0';
    }
    ++writeIndex;
    return writeIndex;
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


size_t StorageDevice_GetName(std::shared_ptr<StorageDevice>* device, char* const name, size_t length) {
    const auto info = (*device)->GetDesc();
    const std::string_view devName = info.name;
    if (name != nullptr) {
        return CopyString(devName.begin(), devName.end(), name, ptrdiff_t(length));
    }
    return devName.size() + 1;
}


size_t StorageDeviceGetSerial(std::shared_ptr<StorageDevice>* device, char* const serial, size_t length) {
    const auto info = (*device)->GetDesc();
    const std::string_view devSerial = info.serial;
    if (serial != nullptr) {
        return CopyString(devSerial.begin(), devSerial.end(), serial, ptrdiff_t(length));
    }
    return devSerial.size() + 1;
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


void ObjectNext(TableIterator* object) {
    std::advance(object->first, 1);
}


bool ObjectValid(TableIterator* object) {
    return object->first != object->second;
}


char* ObjectGetColumnNames(TableIterator* object, size_t startColumn, size_t endColumn) {
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


char* ObjectGetColumnValues(TableIterator* object, size_t startColumn, size_t endColumn) {
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


size_t ObjectSetColumnValues(TableIterator* object, size_t startColumn, size_t endColumn, const char* values) {
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