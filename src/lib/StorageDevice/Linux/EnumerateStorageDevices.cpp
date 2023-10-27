#include "../Common/StorageDevice.hpp"

#include <algorithm>
#include <filesystem>
#include <ranges>
#include <regex>


std::vector<StorageDeviceLabel> EnumerateStorageDevices() {
    namespace fs = std::filesystem;

    const auto isNvme = [](const fs::directory_entry& file) {
        const auto name = file.path().filename().string();
        static const auto namePattern = std::regex("nvme[0-9]+");
        return std::regex_match(name, namePattern);
    };

    auto blockFiles = fs::directory_iterator("/dev");

    std::vector<StorageDeviceLabel> devices;
    for (auto& file : blockFiles) {
        if (isNvme(file)) {
            devices.emplace_back(file.path().string(), eStorageDeviceInterface::NVME);
        }
    }

    return devices;
}