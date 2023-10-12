
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
    const auto bytes = std::as_bytes(std::span(password));
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


void AddCmdHelp(App&, CLI::App& cli) {
    auto cmd = cli.add_subcommand("help", "Print this help message.");
    cmd->callback([&] {
        std::cout << cli.get_formatter()->make_help(&cli, "", CLI::AppFormatMode::Normal) << std::endl;
    });
}


void AddCmdStackReset(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("stack-reset", "Resets the current communication stream. Does not change state.");
    cmd->callback([&app] {
        app.StackReset();
    });
}


void AddCmdReset(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("reset", "Resets the device as if power-cycled. Does not change state.");
    cmd->callback([&app] {
        app.Reset();
    });
}


void AddCmdStart(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("start", "Starts a session with a service provider.");
    cmd->callback([&app] {
        const auto securityProviders = app.GetSecurityProviders();
        size_t index = 0;
        for (auto& [uid, name] : securityProviders) {
            if (name) {
                std::cout << std::format("[{}] {}", ++index, *name) << std::endl;
            }
            else {
                std::cout << std::format("[{}] {:#018x}", ++index, uint64_t(uid)) << std::endl;
            }
        }
        std::cout << std::format("Select SP ({}-{}): ", 1, securityProviders.size());
        std::string s;
        getline(std::cin, s);
        index = size_t(std::atoi(s.data()) - 1);
        if (index < securityProviders.size()) {
            const auto uid = securityProviders[index].uid;
            app.Start(uid);
        }
        else {
            std::cout << "Select a valid number." << std::endl;
        }
    });
}


void AddCmdAuth(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("auth", "Authenticates with an authority.");
    cmd->callback([&app] {
        const auto authorities = app.GetAuthorities();
        size_t index = 0;
        for (auto& [uid, name] : authorities) {
            if (name) {
                std::cout << std::format("[{}] {}", ++index, *name) << std::endl;
            }
            else {
                std::cout << std::format("[{}] {:#018x}", ++index, uint64_t(uid)) << std::endl;
            }
        }
        std::cout << std::format("Select authority ({}-{}): ", 1, authorities.size());
        std::string s;
        getline(std::cin, s);
        index = size_t(std::atoi(s.data()) - 1);
        if (index < authorities.size()) {
            const auto uid = authorities[index].uid;
            const auto password = ReadPassword("Password: ");
            app.Authenticate(uid, password);
        }
        else {
            std::cout << "Select a valid number." << std::endl;
        }
    });
}


void AddCmdEnd(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("end", "End session with current service provider.");
    cmd->callback([&app] {
        app.End();
    });
}


void AddCmdRevert(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("revert", "Revert the device to Original Manufacturing State. All data is unrecoverably lost.");
    cmd->callback([&] {
        try {
            const auto password = ReadPassword("PSID password: ");
            app.Revert(password);
        }
        catch (std::exception& ex) {
            std::cout << ex.what() << std::endl;
        }
    });
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: SEDManager <device>" << std::endl;
        return 1;
    }
    std::string_view device = argv[1];
    try {
        App app{ device };

        CLI::App cli;
        bool hasExited = false;

        int lockingRange = 0;
        uint64_t startLba = 0;
        uint64_t lengthLba = 0;
        bool enabled = true;

        AddCmdHelp(app, cli);
        AddCmdStart(app, cli);
        AddCmdAuth(app, cli);
        AddCmdEnd(app, cli);
        AddCmdStackReset(app, cli);
        AddCmdReset(app, cli);
        AddCmdRevert(app, cli);
        auto cmdExit = cli.add_subcommand("exit", "Exit the application.");
        cmdExit->callback([&] {
            hasExited = true;
        });

        while (!hasExited && std::cin.good()) {
            try {
                std::cout << "sed-manager> ";
                std::string command;
                std::getline(std::cin, command);
                cli.parse(command, false);
            }
            catch (std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        }
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    return 0;
}