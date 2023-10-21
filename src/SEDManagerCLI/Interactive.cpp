#include "Interactive.hpp"

#include "Utility.hpp"

#include <Archive/Types/ValueToJSON.hpp>

#include <CLI/App.hpp>
#include <SEDManager/SEDManager.hpp>

#include <iostream>
#include <ostream>


namespace {

std::optional<Uid> currentSP;
std::vector<Uid> currentAuthorities;


void ClearCurrents() {
    currentSP = std::nullopt;
    currentAuthorities.clear();
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
        app.Start(*maybeSpUid);
        currentSP = *maybeSpUid;
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
        const auto password = GetPassword("Password: ");
        app.Authenticate(*maybeAuthUid, password);
        currentAuthorities.push_back(*maybeAuthUid);
    });
}


void AddCmdEnd(SEDManager& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("end", "End session with current service provider.");
    cmd->callback([&app] {
        app.End();
        ClearCurrents();
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

        const std::vector<std::string> columnNames = { "UID", "Name" };
        std::vector<std::vector<std::string>> rows;
        for (const auto& row : table) {
            rows.push_back({ to_string(row.Id()), app.GetModules().FindName(row.Id()).value_or("") });
        }
        std::cout << FormatTable(columnNames, rows);
    });

    cmdColumns->callback([&app] {
        const auto maybeTableUid = FindOrParseUid(app, tableName);
        if (!maybeTableUid) {
            std::cout << "Cannot find table." << std::endl;
            return;
        }

        const auto table = app.GetTable(*maybeTableUid);
        size_t columnNumber = 0;
        const std::vector<std::string> columnNames = { "Number", "Name", "IsUnique", "Type" };
        std::vector<std::vector<std::string>> rows;
        for (const auto& column : table.GetDesc().columns) {
            rows.push_back({ std::to_string(columnNumber++), column.name, column.isUnique ? "yes" : "", GetTypeStr(column.type) });
        }
        std::cout << FormatTable(columnNames, rows);
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


void AddCmdStackReset(SEDManager& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("stack-reset", "Reset the current communication stream.");
    cmd->callback([&app] {
        app.StackReset();
        ClearCurrents();
    });
}


void AddCmdReset(SEDManager& app, CLI::App& cli) {
    auto cmd = cli.add_subcommand("reset", "Reset the device as if power-cycled.");
    cmd->callback([&app] {
        app.Reset();
        ClearCurrents();
    });
}


void AddCmdGenMEK(SEDManager& app, CLI::App& cli) {
    static std::string rangeName;
    auto cmd = cli.add_subcommand("gen-mek", "Creates a new Media Encryption Key for a locking range, cryptographically erasing that range.");
    cmd->add_option("range", rangeName, "The locking range.");
    cmd->callback([&] {
        const auto maybeRangeUid = FindOrParseUid(app, rangeName);
        if (!maybeRangeUid) {
            std::cout << "Cannot find locking range." << std::endl;
            return;
        }
        app.GenMEK(*maybeRangeUid);
    });
}


void AddCmdGenPIN(SEDManager& app, CLI::App& cli) {
    static std::string credentialObj;
    static uint32_t length = 32;
    auto cmd = cli.add_subcommand("gen-pin", "Creates a new random password for an authority.");
    cmd->add_option("c-pin-obj", credentialObj, "The authority's credential object in C_PIN.");
    cmd->callback([&] {
        const auto maybeCredentialUid = FindOrParseUid(app, credentialObj);
        if (!maybeCredentialUid) {
            std::cout << "Cannot find credential object." << std::endl;
            return;
        }
        app.GenMEK(*maybeCredentialUid);
    });
}


void AddCmdRevert(SEDManager& app, CLI::App& cli) {
    static std::string spName;
    auto cmd = cli.add_subcommand("revert", "Revert an SP to Original Manufacturing State. All data is unrecoverably lost!");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&] {
        const auto maybeSpUid = FindOrParseUid(app, spName);
        if (!maybeSpUid) {
            std::cout << "Cannot find security provider." << std::endl;
            return;
        }
        app.Revert(maybeSpUid.value());
        ClearCurrents();
    });
}


void AddCmdActivate(SEDManager& app, CLI::App& cli) {
    static std::string spName;
    auto cmd = cli.add_subcommand("activate", "Activate an SP that's ben disabled the manufacturer.");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&] {
        const auto maybeSpUid = FindOrParseUid(app, spName);
        if (!maybeSpUid) {
            std::cout << "Cannot find security provider." << std::endl;
            return;
        }
        app.Activate(maybeSpUid.value());
    });
}

} // namespace


namespace interactive {

void AddCommands(SEDManager& app, CLI::App& cli) {
    AddCmdStart(app, cli);
    AddCmdAuth(app, cli);
    AddCmdEnd(app, cli);

    AddCmdTable(app, cli);
    AddCmdGetSet(app, cli);
    AddCmdGenMEK(app, cli);
    AddCmdGenPIN(app, cli);

    AddCmdRevert(app, cli);
    AddCmdActivate(app, cli);

    AddCmdStackReset(app, cli);
    AddCmdReset(app, cli);
}


std::optional<Uid> GetCurrentSP() { return currentSP; }
std::span<const Uid> GetCurrentAuthorities() { return currentAuthorities; }

} // namespace interactive