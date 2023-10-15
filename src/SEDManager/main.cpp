
#include "App.hpp"

#include <Archive/TokenTextArchive.hpp>
#include <Specification/Tables.hpp>

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include <fstream>
#include <iostream>
#include <string_view>
#include <unordered_map>

#ifdef __linux__
    #include <unistd.h>
#elif defined(_WIN32)
    #include <conio.h>
#endif


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
#ifdef __linux__
    std::string_view password = getpass(prompt.data());
    const auto bytes = std::as_bytes(std::span(password));
    return { bytes.begin(), bytes.end() };
#elif defined(_WIN32)
    std::cout << prompt;
    std::vector<std::byte> password;
    char ch;
    while (true) {
        ch = _getch();
        if (ch != '\r' && ch != '\n') {
            password.push_back(static_cast<std::byte>(ch));
        }
        else {
            break;
        }
    }
    while (0 < std::cin.readsome(&ch, 1))
    {}
    std::cout << std::endl;
    return password;
#else
    static_assert(false, "Password reading is not implemented for platform.");
#endif
}


std::string FormatNamed(const NamedObject& obj) {
    return std::format("{:#018x} {}", uint64_t(obj.uid), obj.name.value_or(""));
}


std::string FormatUid(const Uid& obj) {
    return std::format("{:#018x}", uint64_t(obj));
}


void AddCmdHelp(App&, CLI::App& cli) {
    auto cmd = cli.add_subcommand("help", "Print this help message.");
    cmd->callback([&] {
        std::cout << cli.get_formatter()->make_help(&cli, "", CLI::AppFormatMode::Normal) << std::endl;
    });
}


void AddCmdStackReset(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("stack-reset", "Reset the current communication stream. Does not change state.");
    cmd->callback([&app] {
        app.StackReset();
    });
}


void AddCmdReset(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("reset", "Reset the device as if power-cycled. Does not change state.");
    cmd->callback([&app] {
        app.Reset();
    });
}


void AddCmdStart(App& app, CLI::App& cli) {
    static std::string spName;

    auto cmd = cli.add_subcommand("start", "Start a session with a service provider.");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&app] {
        const auto maybeSpUid = GetUidOrHex(spName);
        if (!maybeSpUid) {
            std::cout << "Cannot find security provider." << std::endl;
            return;
        }
        app.Start(maybeSpUid.value());
    });
}


void AddCmdAuth(App& app, CLI::App& cli) {
    static std::string authName;

    auto cmd = cli.add_subcommand("auth", "Authenticate with an authority.");
    cmd->add_option("authority", authName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&app] {
        const auto maybeAuthUid = GetUidOrHex(authName);
        if (!maybeAuthUid) {
            std::cout << "Cannot find authority." << std::endl;
            return;
        }
        const auto password = ReadPassword("Password: ");
        app.Authenticate(maybeAuthUid.value(), password);
    });
}


void AddCmdList(App& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("list", "List different objects.");

    auto cmdSp = cmd->add_subcommand("sps", "List security providers.");
    auto cmdAuth = cmd->add_subcommand("auths", "List security authorities.");
    auto cmdTables = cmd->add_subcommand("tables", "List table.");

    cmdSp->callback([&app] {
        const auto& sps = app.GetSecurityProviders();
        for (const auto& v : sps) {
            std::cout << FormatNamed(v) << std::endl;
        }
    });

    cmdAuth->callback([&app] {
        const auto& auth = app.GetAuthorities();
        for (const auto& v : auth) {
            std::cout << FormatNamed(v) << std::endl;
        }
    });

    cmdTables->callback([&app] {
        const auto& tables = app.GetTables();
        for (const auto& v : tables) {
            std::cout << FormatNamed(v) << std::endl;
        }
    });
}


void AddCmdTable(App& app, CLI::App& cli) {
    static std::string tableName;
    static std::string rowName;
    static uint32_t column = 0;

    auto cmd = cli.add_subcommand("table", "Manipulate tables.");
    auto cmdColumns = cmd->add_subcommand("columns", "List the columns of the table.");
    auto cmdRows = cmd->add_subcommand("rows", "List the rows of the table.");
    auto cmdGet = cmd->add_subcommand("get", "Get a cell from a table.");

    cmdRows->add_option("table", tableName, "The table to list the rows of.");
    cmdColumns->add_option("table", tableName, "The table to list the columns of.");
    cmdGet->add_option("row", rowName, "The row to get the cells of.");
    cmdGet->add_option("column", column, "The row to get the cells of.");

    cmdRows->callback([&app] {
        const auto maybeTableUid = GetUidOrHex(tableName);
        if (!maybeTableUid) {
            std::cout << "Cannot find table." << std::endl;
            return;
        }
        const auto table = app.GetTable(*maybeTableUid);
        for (const auto& row : table) {
            std::cout << FormatUid(row.Id()) << "  " << GetName(row.Id()).value_or("") << std::endl;
        }
    });

    cmdColumns->callback([&app] {
        const auto maybeTableUid = GetUidOrHex(tableName);
        if (!maybeTableUid) {
            std::cout << "Cannot find table." << std::endl;
            return;
        }

        const auto& desc = GetTableDesc(*maybeTableUid);
        size_t columnNumber = 0;
        constexpr std::string_view lineFormat = "{:>5} | {:<32} | {:>8} | {:<32}";
        std::cout << std::format(lineFormat, "Index", "Name", "IsUnique", "Type") << std::endl;
        for (const auto& [name, isUnique, type] : desc.columns) {
            std::cout << std::format(lineFormat, columnNumber++, name, isUnique ? "yes" : "no", type) << std::endl;
        }
    });

    cmdGet->callback([&app] {
        const auto maybeRowUid = GetUidOrHex(rowName);
        if (!maybeRowUid) {
            std::cout << "Cannot find row." << std::endl;
            return;
        }
        const Value value = app.Get(*maybeRowUid, column);
        if (!value.HasValue()) {
            std::cout << "<empty>" << std::endl;
        }
        else {
            TokenTextArchive ar(std::cout);
            ar(value);
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

        AddCmdHelp(app, cli);
        AddCmdStart(app, cli);
        AddCmdAuth(app, cli);
        AddCmdList(app, cli);
        AddCmdTable(app, cli);
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
                std::cout << "sedmanager";
                std::cout << "> ";
                std::string command;
                std::getline(std::cin, command);
                cli.parse(command, false);
            }
            catch (std::exception& ex) {
                std::cout << "Error: " << ex.what() << std::endl;
            }
        }
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    return 0;
}