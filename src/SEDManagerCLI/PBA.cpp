#include "PBA.hpp"

#include "Utility.hpp"
#include <asyncpp/join.hpp>

#include <Messaging/Native.hpp>
#include <StorageDevice/StorageDevice.hpp>

#include <EncryptedDevice/EncryptedDevice.hpp>


using namespace sedmgr;


namespace {

void UnlockDevice(std::shared_ptr<StorageDevice> device);

} // namespace


PBA::PBA() {}


int PBA::Run() {
    const auto deviceLabels = EnumerateStorageDevices();

    for (auto& deviceLabel : deviceLabels) {
        std::shared_ptr<StorageDevice> device;
        if (deviceLabel.interface == eStorageDeviceInterface::NVME) {
            device = std::make_shared<NvmeDevice>(deviceLabel.path);
        }
        if (device) {
            UnlockDevice(device);
        }
    }

    return 0;
}


namespace {


std::optional<EncryptedDevice> ConnectDevice(std::shared_ptr<StorageDevice> device) {
    try {
        return EncryptedDevice(device);
    }
    catch (std::exception&) {
        return std::nullopt;
    }
}


bool IsLockingEnabled(const EncryptedDevice& manager) {
    const auto desc = manager.GetDesc();
    if (!desc.lockingDesc) {
        return false;
    }
    return desc.lockingDesc->lockingEnabled;
}


std::string GetDeviceName(std::shared_ptr<StorageDevice> device) {
    if (const auto nvmeDevice = std::dynamic_pointer_cast<NvmeDevice>(device)) {
        const auto identity = nvmeDevice->IdentifyController();
        return identity.modelNumber;
    }
    return "<name unknown>";
}


SimpleSession StartLockingSession(EncryptedDevice& manager) {
    const auto lockingSpUid = Unwrap(manager.GetModules().FindUid("SP::Locking"), "could not find Locking SP");

    try {
        return join(manager.Login(lockingSpUid));
    }
    catch (SecurityProviderBusyError& ex) {
        // Do a stack reset and retry, maybe a previous session was not terminated properly.
        join(manager.StackReset());
        return join(manager.Login(lockingSpUid));
    }
}


std::optional<std::string> UnwrapCommonName(Value commonName) {
    try {
        return value_cast<std::string>(commonName);
    }
    catch (std::exception&) {
        return std::nullopt;
    }
}


std::string FormatName(std::string_view name, const std::optional<std::string> commonName) {
    return commonName && !commonName->empty() ? std::format("{} / '{}'", name, *commonName) : std::string(name);
}


std::optional<UID> FindAuthority(SimpleSession& manager, std::string_view name) {
    const auto lockingSpUid = Unwrap(manager.GetModules().FindUid("SP::Locking"), "could not find Locking SP");
    const auto maybeUid = ParseObjectRef(manager.GetModules(), std::format("Authority::{}", name), lockingSpUid);
    if (maybeUid) {
        return maybeUid;
    }
    const auto authorityTableUid = Unwrap(manager.GetModules().FindUid("Authority"), "could not find Authority table");
    const auto authorityUids = manager.GetTableRows(authorityTableUid);
    while (const auto authority = join(authorityUids)) {
        try {
            const auto commonName = UnwrapCommonName(join(manager.GetValue(*authority, 2)));
            if (commonName && *commonName == name) {
                return *authority;
            }
        }
        catch (std::exception&) {
            continue;
        }
    }
    return std::nullopt;
}


std::optional<UID> TryGetUser(SimpleSession& session) {
    constexpr auto maxTries = 5;

    for (auto i = 0; i < maxTries; ++i) {
        std::cout << "Login: ";
        std::string userName;
        std::getline(std::cin, userName);
        if (!std::cin.good()) {
            std::cin.clear();
            std::cout << "Cancelled." << std::endl;
            return std::nullopt;
        }
        const auto authority = FindAuthority(session, userName);
        if (authority) {
            return authority;
        }
        std::cout << "Cannot find user, try again..." << std::endl;
    }
    return std::nullopt;
}


bool TryLoginUser(SimpleSession& session, UID authority) {
    constexpr auto maxTries = 5;

    for (auto i = 0; i < maxTries; ++i) {
        try {
            const auto password = GetPassword("Password: ");
            if (!std::cin.good()) {
                std::cin.clear();
                std::cout << "Cancelled." << std::endl;
                return false;
            }
            join(session.Authenticate(authority, password));
            return true;
        }
        catch (PasswordError&) {
            std::cout << "Wrong password, try again..." << std::endl;
        }
    }
    return false;
}


void TryUnlockRanges(SimpleSession& manager) {
    const auto lockingSp = Unwrap(manager.GetModules().FindUid("SP::Locking"), "could not find Locking SP");
    const auto lockingTableUid = Unwrap(manager.GetModules().FindUid("Locking"), "could not find Locking table");
    auto lockingRangeUids = manager.GetTableRows(lockingTableUid);

    while (const auto lockingRange = join(lockingRangeUids)) {
        const auto name = FormatObjectRef(manager.GetModules(), *lockingRange, lockingSp);
        const auto commonName = UnwrapCommonName(join(manager.GetValue(*lockingRange, 2)));

        bool rdUnlocked = false;
        bool wrUnlocked = false;
        try {
            join(manager.SetValue(*lockingRange, 7, false));
            rdUnlocked = true;
        }
        catch (NotAuthorizedError& ex) {
            // Expected.
        }
        try {
            join(manager.SetValue(*lockingRange, 8, false));
            wrUnlocked = true;
        }
        catch (NotAuthorizedError& ex) {
            // Expected.
        }
        if (rdUnlocked || wrUnlocked) {
            std::cout << std::format("Unlocked ({}{}) {}!", rdUnlocked ? "R" : "", wrUnlocked ? "W" : "", FormatName(name, commonName))
                      << std::endl;
        }
    }
}


void TryDoMBR(SimpleSession& session) {
    const auto mbrControlTableUid = Unwrap(session.GetModules().FindUid("MBRControl"), "could not find MBRControl table");
    try {
        join(session.SetValue(mbrControlTableUid, 2, 1));
        std::cout << "MBR Done!" << std::endl;
    }
    catch (std::exception&) {
        // Ignore:
        // Some devices don't even have MBR shadowing.
    }
}


void UnlockDevice(std::shared_ptr<StorageDevice> device) {
    std::optional<EncryptedDevice> maybeEncryptedDevice = ConnectDevice(device);
    if (!maybeEncryptedDevice) {
        // Device does not support TCG specifications.
        // Ignore device.
        return;
    }
    auto& encryptedDevice = *maybeEncryptedDevice;

    if (!IsLockingEnabled(encryptedDevice)) {
        // Device does not have locking enabled.
        // Ignore device.
        return;
    }

    std::cout << std::format("Unlock '{}':", GetDeviceName(device)) << std::endl;

    try {
        auto session = StartLockingSession(encryptedDevice);
        const auto user = TryGetUser(session);
        if (!user) {
            return;
        }
        const auto success = TryLoginUser(session, *user);
        if (!success) {
            return;
        }
        TryUnlockRanges(session);
        TryDoMBR(session);
    }
    catch (std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
}

} // namespace