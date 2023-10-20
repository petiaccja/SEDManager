
#include "SEDManager.hpp"

#include <Archive/Types/ValueToJSON.hpp>

#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include <fstream>
#include <iostream>

#ifdef __linux__
    #include <unistd.h>
#elif defined(_WIN32)
    #include <conio.h>
#endif


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


std::vector<std::string_view> SplitName(std::string_view name) {
    std::vector<std::string_view> sections;
    size_t pos = 0;
    while (pos <= name.size()) {
        const auto start = pos;
        pos = name.find("::", pos);
        sections.push_back(name.substr(start, pos));
        pos = std::max(pos + 2, pos);
    }
    return sections;
}


std::optional<Uid> FindOrParseUid(SEDManager& app, std::string_view nameOrUid) {
    const auto maybeUid = app.GetModules().FindUid(nameOrUid);
    if (maybeUid) {
        return *maybeUid;
    }
    try {
        size_t index = 0;
        const uint64_t parsedUid = std::stoull(std::string(nameOrUid), &index, 16);
        if (index == nameOrUid.size()) {
            return Uid(parsedUid);
        }
    }
    catch (...) {
        // Fallthrough
    }
    throw std::invalid_argument("failed to find name or parse UID");
}


void AddCmdHelp(SEDManager&, CLI::App& cli) {
    auto cmd = cli.add_subcommand("help", "Print this help message.");
    cmd->callback([&] {
        std::cout << cli.get_formatter()->make_help(&cli, "", CLI::AppFormatMode::Normal) << std::endl;
    });
}


void AddCmdStackReset(SEDManager& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("stack-reset", "Reset the current communication stream. Does not change state.");
    cmd->callback([&app] {
        app.StackReset();
    });
}


void AddCmdReset(SEDManager& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("reset", "Reset the device as if power-cycled. Does not change state.");
    cmd->callback([&app] {
        app.Reset();
    });
}


void AddCmdStart(SEDManager& app, CLI::App& cli) {
    static std::string spName;

    auto cmd = cli.add_subcommand("start", "Start a session with a service provider.");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&app] {
        const auto maybeSpUid = FindOrParseUid(app, spName);
        if (!maybeSpUid) {
            std::cout << "Cannot find security provider." << std::endl;
            return;
        }
        app.Start(maybeSpUid.value());
    });
}


void AddCmdAuth(SEDManager& app, CLI::App& cli) {
    static std::string authName;

    auto cmd = cli.add_subcommand("auth", "Authenticate with an authority.");
    cmd->add_option("authority", authName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&app] {
        const auto maybeAuthUid = FindOrParseUid(app, authName);
        if (!maybeAuthUid) {
            std::cout << "Cannot find authority." << std::endl;
            return;
        }
        const auto password = ReadPassword("Password: ");
        app.Authenticate(maybeAuthUid.value(), password);
    });
}


void AddCmdList(SEDManager& app, CLI::App& cli) {
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


void AddCmdTable(SEDManager& app, CLI::App& cli) {
    static std::string tableName;

    auto cmdColumns = cli.add_subcommand("columns", "List the columns of the table.");
    auto cmdRows = cli.add_subcommand("rows", "List the rows of the table.");

    cmdRows->add_option("table", tableName, "The table to list the rows of.");
    cmdColumns->add_option("table", tableName, "The table to list the columns of.");

    cmdRows->callback([&app] {
        const auto maybeTableUid = FindOrParseUid(app, tableName);
        if (!maybeTableUid) {
            std::cout << "Cannot find table." << std::endl;
            return;
        }
        const auto table = app.GetTable(*maybeTableUid);
        for (const auto& row : table) {
            std::cout << to_string(row.Id()) << "  " << app.GetModules().FindName(row.Id()).value_or("") << std::endl;
        }
    });

    cmdColumns->callback([&app] {
        const auto maybeTableUid = FindOrParseUid(app, tableName);
        if (!maybeTableUid) {
            std::cout << "Cannot find table." << std::endl;
            return;
        }

        const auto table = app.GetTable(*maybeTableUid);
        size_t columnNumber = 0;
        constexpr std::string_view lineFormat = "{:>5} | {:<32} | {:>8} | {}";
        std::cout << std::format(lineFormat, "Index", "Name", "IsUnique", "Type") << std::endl;
        for (const auto& [name, isUnique, type] : table.GetDesc().columns) {
            std::cout << std::format(lineFormat, columnNumber++, name, isUnique ? "yes" : "no", GetTypeStr(type)) << std::endl;
        }
    });
}

void AddCmdGetSet(SEDManager& app, CLI::App& cli) {
    static std::string rowName;
    static uint32_t column = 0;
    static std::string jsonValue;

    auto cmdGet = cli.add_subcommand("get", "Get a cell from a table.");
    auto cmdSet = cli.add_subcommand("set", "Set a cell in a table to new value.");

    cmdGet->add_option("row", rowName, "The row to get the cells of.");
    cmdGet->add_option("column", column, "The column to get.");
    cmdSet->add_option("row", rowName, "The row to set the cells of.");
    cmdSet->add_option("column", column, "The column to set.");
    cmdSet->add_option("value", jsonValue, "The new value of the cell.");


    static const auto parse = [&]() -> std::optional<std::tuple<Uid, Uid, uint32_t>> {
        const auto& rowNameSections = SplitName(rowName);
        if (rowNameSections.size() != 2) {
            std::cout << "Specify object to get as 'Table::Object', either with names or UIDs." << std::endl;
            return std::nullopt;
        }

        const auto maybeTableUid = FindOrParseUid(app, rowNameSections[0]);
        const auto maybeRowUid = FindOrParseUid(app, rowName);
        if (!maybeTableUid) {
            std::cout << "Table must be a valid name or a UID." << std::endl;
            return std::nullopt;
        }
        if (!maybeRowUid) {
            std::cout << "Object must be a valid name or a UID." << std::endl;
            return std::nullopt;
        }
        return std::tuple{ *maybeTableUid, *maybeRowUid, column };
    };

    cmdGet->callback([&app] {
        const auto parsed = parse();
        if (!parsed) {
            return;
        }
        const auto& [tableUid, rowUid, column] = *parsed;
        const auto object = app.GetObject(tableUid, rowUid);
        if (!(column < object.size())) {
            throw std::invalid_argument("Column index is out of bounds.");
        }
        const auto value = *object[column];
        if (!value.HasValue()) {
            std::cout << "<empty>" << std::endl;
        }
        else {
            const auto json = ValueToJSON(value, object.GetDesc()[column].type);
            std::cout << json.dump() << std::endl;
        }
    });

    cmdSet->callback([&app] {
        const auto parsed = parse();
        if (!parsed) {
            return;
        }
        const auto [tableUid, rowUid, column] = *parsed;
        const auto object = app.GetObject(tableUid, rowUid);
        const auto value = JSONToValue(nlohmann::json::parse(jsonValue), object.GetDesc()[column].type);
        object[column] = value;
    });
}


void AddCmdEnd(SEDManager& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("end", "End session with current service provider.");
    cmd->callback([&app] {
        app.End();
    });
}


void AddCmdRevert(SEDManager& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("revert", "Revert the device to Original Manufacturing State. All data is unrecoverably lost.");
    cmd->callback([&] {
        try {
            throw std::logic_error("not implemented");
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
        SEDManager app{ device };

        CLI::App cli;
        bool hasExited = false;

        AddCmdHelp(app, cli);
        AddCmdStart(app, cli);
        AddCmdAuth(app, cli);
        AddCmdList(app, cli);
        AddCmdTable(app, cli);
        AddCmdGetSet(app, cli);
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