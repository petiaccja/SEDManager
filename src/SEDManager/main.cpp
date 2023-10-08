
#include "App.hpp"

#include <TPerLib/Core/Tables.hpp>
#include <TPerLib/Serialization/Utility.hpp>

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include <fstream>
#include <iostream>
#include <ranges>
#include <string_view>
#include <unordered_map>


std::string_view ConvertRawCharacters(std::ranges::contiguous_range auto&& r) {
    return std::string_view(std::ranges::begin(r), std::ranges::end(r));
}


std::string_view GetSSCName(const StorageClassFeatureDesc& sscDesc) {
    if (std::holds_alternative<Opal2FeatureDesc>(sscDesc)) {
        return "Opal V2";
    }
    else if (std::holds_alternative<Pyrite1FeatureDesc>(sscDesc)) {
        return "Pyrite V1";
    }
    else if (std::holds_alternative<Pyrite2FeatureDesc>(sscDesc)) {
        return "Pyrite V2";
    }
    else if (std::holds_alternative<OpaliteFeatureDesc>(sscDesc)) {
        return "Opalite";
    }
    else {
        return "Unkown storage subsystem class";
    }
}


std::string_view GetComIdStateStr(eComIdState state) {
    switch (state) {
        case eComIdState::INVALID: return "invalid";
        case eComIdState::INACTIVE: return "inactive";
        case eComIdState::ISSUED: return "issued";
        case eComIdState::ASSOCIATED: return "associated";
    }
    return "unknown";
};


void PrintCapabilities(const TPerDesc& desc) {
    std::cout << std::format("  {}", GetSSCName(desc.sscDesc)) << std::endl;

    if (desc.tperDesc) {
        std::cout << "  TPer:" << std::endl;
        std::cout << std::format("    ComID management supported: {}", desc.tperDesc->comIdMgmtSupported) << std::endl;
    }
    if (desc.lockingDesc) {
        std::cout << "  Locking:" << std::endl;
        std::cout << std::format("    Locking supported:          {}", desc.lockingDesc->lockingSupported) << std::endl;
        std::cout << std::format("    Locking enabled:            {}", desc.lockingDesc->lockingEnabled) << std::endl;
        std::cout << std::format("    Locked:                     {}", desc.lockingDesc->locked) << std::endl;
        std::cout << std::format("    Media encryption supported: {}", desc.lockingDesc->mediaEncryption) << std::endl;
        std::cout << std::format("    Shadow MBR done:            {}", desc.lockingDesc->mbrDone) << std::endl;
        std::cout << std::format("    Shadow MBR enabled:         {}", desc.lockingDesc->mbrEnabled) << std::endl;
        std::cout << std::format("    Shadow MBR supported:       {}", desc.lockingDesc->mbrSupported) << std::endl;
    }
    if (!std::holds_alternative<std::monostate>(desc.sscDesc)) {
        std::cout << "  SSC:" << std::endl;
        std::visit([](const auto& arg) {
            if constexpr (!std::is_convertible_v<decltype(arg), std::monostate>) {
                std::cout << std::format("    Base ComID:                 {}", arg.baseComId) << std::endl;
                std::cout << std::format("    Num ComIDs:                 {}", arg.numComIds) << std::endl;
                std::cout << std::format("    Initial C_PIN:              {}", arg.initialCPinSidIndicator) << std::endl;
                std::cout << std::format("    C_PIN revert behavior:      {}", arg.cPinSidRevertBehavior) << std::endl;
            }
        },
                   desc.sscDesc);
    }
}


void PrintProperies(const std::unordered_map<std::string, uint32_t>& properties) {
    std::cout << "TPer properties: " << std::endl;
    for (const auto& [name, value] : properties) {
        std::cout << std::format("  {} = {}", name, value) << std::endl;
    }
    std::cout << std::endl;
}


std::vector<std::byte> ReadPassword(std::string_view prompt) {
    std::string_view password = getpass(prompt.data());
    const auto bytes = std::as_bytes(BytesView(password));
    return { bytes.begin(), bytes.end() };
}


Uid GetRange(int number) {
    switch (number) {
        case 0: return eRows_Locking::GlobalRange;
        case 1: return opal::eRows_Locking::Range1;
        case 2: return opal::eRows_Locking::Range2;
        case 3: return opal::eRows_Locking::Range3;
        case 4: return opal::eRows_Locking::Range4;
        case 5: return opal::eRows_Locking::Range5;
        case 6: return opal::eRows_Locking::Range6;
        case 7: return opal::eRows_Locking::Range7;
        case 8: return opal::eRows_Locking::Range8;
        default: throw std::invalid_argument("invalid locking range (allowed 0=global, 1..8)");
    }
}


int main() {
    try {
        App app{ "/dev/nvme1" };

        CLI::App cliApp;
        bool hasExited = false;

        int lockingRange = 0;
        uint64_t startLba = 0;
        uint64_t lengthLba = 0;
        bool enabled = true;

        auto cmdHelp = cliApp.add_subcommand("help", "Print this help message.");
        auto cmdGetMsid = cliApp.add_subcommand("get-msid", "Print the MSID password.");
        auto cmdSetSidPw = cliApp.add_subcommand("set-sid-pw", "Change the SID password.");
        auto cmdActivateLocking = cliApp.add_subcommand("activate-locking", "Activate locking feature. This does not lock the drive.");
        auto cmdConfigRange = cliApp.add_subcommand("config-range", "Configure and lock down all LBAs. All data is unrecoverably lost.");
        auto cmdLock = cliApp.add_subcommand("lock", "Locks the specified range. Data is not modified.");
        auto cmdUnlock = cliApp.add_subcommand("unlock", "Unlocks the specified range. Data is not modified.");
        auto cmdRevert = cliApp.add_subcommand("revert", "Revert the device to Original Manufacturing State. All data is unrecoverably lost.");
        auto cmdExit = cliApp.add_subcommand("exit", "Exit the application.");

        cmdConfigRange->add_option("--range", lockingRange, "Which range to configure.")->default_val(0);
        cmdConfigRange->add_option("--start", startLba, "Start LBA of the range.")->default_val(0);
        cmdConfigRange->add_option("--length", lengthLba, "Length of range in LBAs.")->default_val(0);
        cmdConfigRange->add_option("--enabled", enabled, "1 to enable, 0 to disable locking of range.")->default_val(true);

        cmdLock->add_option("--range", lockingRange, "Which range to lock.")->default_val(0);
        cmdUnlock->add_option("--range", lockingRange, "Which range to unlock.")->default_val(0);


        cmdHelp->callback([&] {
            std::cout << cliApp.get_formatter()->make_help(&cliApp, "", CLI::AppFormatMode::Normal) << std::endl;
        });

        cmdGetMsid->callback([&] {
            try {
                const auto msid = app.GetMSIDPassword();
                std::cout << "MSID password: " << std::string_view(reinterpret_cast<const char*>(msid.data()), msid.size()) << std::endl;
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdSetSidPw->callback([&] {
            try {
                const auto currentPw = ReadPassword("Current SID password:");
                const auto newPw = ReadPassword("New SID password:");
                const auto newPwAgain = ReadPassword("New SID password again:");
                if (newPw != newPwAgain) {
                    std::cout << "Passwords don't match. Try again." << std::endl;
                }
                else {
                    app.SetSIDPassword(currentPw, newPw);
                }
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdActivateLocking->callback([&] {
            try {
                const auto password = ReadPassword("SID password:");
                bool activated = app.ActivateLocking(password);
                std::cout << (activated ? "Locking activated." : "Locking already active.") << std::endl;
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdConfigRange->callback([&] {
            try {
                const auto password = ReadPassword("Admin1 password:");
                if (lockingRange == 0) {
                    app.ConfigureLockingRange(password, GetRange(lockingRange), enabled, enabled);
                }
                else {
                    app.ConfigureLockingRange(password, GetRange(lockingRange), enabled, enabled, startLba, lengthLba);
                }
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdLock->callback([&] {
            try {
                const auto password = ReadPassword("Admin1 password:");
                app.SetLockingRange(password, GetRange(lockingRange), true, true);
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdUnlock->callback([&] {
            try {
                const auto password = ReadPassword("Admin1 password:");
                app.SetLockingRange(password, GetRange(lockingRange), false, false);
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdRevert->callback([&] {
            try {
                const auto password = ReadPassword("PSID password: ");
                app.Revert(password);
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        });

        cmdExit->callback([&] {
            hasExited = true;
        });

        while (!hasExited) {
            try {
                std::cout << "sed-manager> ";
                std::string command;
                std::getline(std::cin, command);
                cliApp.parse(command, false);
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        }
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}