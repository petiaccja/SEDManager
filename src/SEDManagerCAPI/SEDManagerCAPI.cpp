#include <MockDevice/MockDevice.hpp>

#include <SEDManager/SEDManager.hpp>


static std::string lastErrorMessage;
static constexpr std::string_view mockDevicePath = "/dev/mock_device";


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
    labels.emplace_back(std::string(mockDevicePath), sedmgr::eStorageDeviceInterface::OTHER);
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


std::shared_ptr<sedmgr::StorageDevice>* CreateStorageDevice(const char* path) {
    if (path == mockDevicePath) {
        const auto device = std::make_shared<sedmgr::MockDevice>();
        return new std::shared_ptr<sedmgr::StorageDevice>(device);
    }
    try {
        const auto device = std::make_shared<sedmgr::NvmeDevice>(path);
        const auto identity = device->IdentifyController(); // Test if device is actually NVMe.
        return new std::shared_ptr<sedmgr::StorageDevice>(device);
    }
    catch (...) {
        SetLastError(std::current_exception());
    }
    return nullptr;
}


size_t StorageDeviceGetName(std::shared_ptr<sedmgr::StorageDevice>* device, char* const name, size_t length) {
    const auto info = (*device)->GetDesc();
    const std::string_view devName = info.name;
    if (name != nullptr) {
        return CopyString(devName.begin(), devName.end(), name, ptrdiff_t(length));
    }
    return devName.size() + 1;
}


size_t StorageDeviceGetSerial(std::shared_ptr<sedmgr::StorageDevice>* device, char* const serial, size_t length) {
    const auto info = (*device)->GetDesc();
    const std::string_view devSerial = info.serial;
    if (serial != nullptr) {
        return CopyString(devSerial.begin(), devSerial.end(), serial, ptrdiff_t(length));
    }
    return devSerial.size() + 1;
}


void ReleaseStorageDevice(std::shared_ptr<sedmgr::StorageDevice>* device) {
    delete device;
}


sedmgr::SEDManager* CreateSEDManager(std::shared_ptr<sedmgr::StorageDevice>* device) {
    try {
        return new sedmgr::SEDManager(*device);
    }
    catch (...) {
        SetLastError(std::current_exception());
    }
    return nullptr;
}


void ReleaseSEDManager(sedmgr::SEDManager* manager) {
    delete manager;
}
}