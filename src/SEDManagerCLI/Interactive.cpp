#include "Interactive.hpp"

#include "Utility.hpp"

#include <Archive/Types/ValueToJSON.hpp>

#include <CLI/App.hpp>
#include <SEDManager/SEDManager.hpp>

#include <iostream>
#include <ostream>


namespace {

std::optional<Uid> currentSP;
std::unordered_set<Uid> currentAuthorities;


void ClearCurrents() {
    currentSP = std::nullopt;
    currentAuthorities.clear();
}


void AddCmdStart(SEDManager& app, CLI::App& cli) {
    static std::string spName;

    auto cmd = cli.add_subcommand("start", "Start a session with a service provider.");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&app] {
        const auto spUid = Unwrap(FindOrParseUid(app, "SP::" + spName, currentSP), "cannot find security provider");
        app.Start(spUid);
        currentSP = spUid;
    });
}


void AddCmdAuth(SEDManager& app, CLI::App& cli) {
    static std::string authName;

    auto cmd = cli.add_subcommand("auth", "Authenticate with an authority.");
    cmd->add_option("authority", authName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&app] {
        const auto authUid = Unwrap(FindOrParseUid(app, "Authority::" + authName, currentSP), "cannot find authority");
        const auto password = GetPassword("Password: ");
        app.Authenticate(authUid, password);
        currentAuthorities.insert(authUid);
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
        const auto tableUid = Unwrap(FindOrParseUid(app, tableName, currentSP), "cannot find table");
        const auto table = app.GetTable(tableUid);

        const std::vector<std::string> columnNames = { "UID", "Name" };
        std::vector<std::vector<std::string>> rows;
        for (const auto& row : table) {
            rows.push_back({ to_string(row.Id()), app.GetModules().FindName(row.Id(), currentSP).value_or("") });
        }
        std::cout << FormatTable(columnNames, rows);
    });

    cmdColumns->callback([&app] {
        const auto tableUid = Unwrap(FindOrParseUid(app, tableName, currentSP), "cannot find table");
        const auto table = app.GetTable(tableUid);

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
    static int32_t column = 0;
    static std::string jsonValue;

    auto cmdGet = cli.add_subcommand("get", "Get a cell from a table.");
    auto cmdSet = cli.add_subcommand("set", "Set a cell in a table to new value.");

    cmdGet->add_option("object", rowName, "The object to get the cells of. Format as 'Table::Object'.");
    cmdGet->add_option("column", column, "The column to get.")->default_val(-1);
    cmdSet->add_option("object", rowName, "The object to set the cells of. Format as 'Table::Object'.");
    cmdSet->add_option("column", column, "The column to set.");
    cmdSet->add_option("value", jsonValue, "The new value of the cell.");


    static const auto parse = [&]() -> std::optional<std::tuple<Uid, Uid, int32_t>> {
        const auto& rowNameSections = SplitName(rowName);
        if (rowNameSections.size() != 2) {
            throw std::invalid_argument("specify object as 'Table::Object'");
        }

        const auto tableUid = Unwrap(FindOrParseUid(app, rowNameSections[0], currentSP), "cannot find table");
        const auto maybeRowUid = FindOrParseUid(app, rowName, currentSP).value_or(FindOrParseUid(app, rowNameSections[1], currentSP).value_or(0));
        if (maybeRowUid == Uid(0)) {
            throw std::invalid_argument("cannot find object");
        }
        auto copy = column;
        column = -1;
        return std::tuple{ tableUid, maybeRowUid, copy };
    };

    cmdGet->callback([&app] {
        const auto parsed = parse();
        if (!parsed) {
            return;
        }
        const auto& [tableUid, rowUid, column] = *parsed;
        const auto object = app.GetObject(tableUid, rowUid);
        if (column < 0) {
            const std::vector<std::string> outColumns = { "Column", "Value" };
            std::vector<std::vector<std::string>> outData;
            size_t idx = 0;
            for (const auto& columnDesc : object.GetDesc()) {
                const auto label = std::format("{}: {}", idx, columnDesc.name);
                try {
                    const auto value = *object[idx];
                    const auto valueStr = value.HasValue() ? ValueToJSON(value, columnDesc.type).dump() : "<empty>";
                    outData.push_back({ label, valueStr });
                }
                catch (std::exception& ex) {
                    outData.push_back({ label, std::string("error: ") + ex.what() });
                }
                ++idx;
            }
            std::cout << FormatTable(outColumns, outData) << std::endl;
        }
        else {
            if (!(column < std::ssize(object))) {
                throw std::invalid_argument("column index is out of bounds.");
            }
            const auto value = *object[column];
            std::cout << (value.HasValue() ? ValueToJSON(value, object.GetDesc()[column].type).dump() : "<empty>") << std::endl;
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
    auto cmd = cli.add_subcommand("gen-mek", "Creates a new Media Encryption Key for a locking range. ERASES RANGE!");
    cmd->add_option("range", rangeName, "The locking range.");
    cmd->callback([&] {
        const auto rangeUid = Unwrap(FindOrParseUid(app, rangeName, currentSP), "cannot find locking range");
        app.GenMEK(rangeUid);
    });
}


void AddCmdGenPIN(SEDManager& app, CLI::App& cli) {
    static std::string credentialObj;
    static uint32_t length = 32;
    auto cmd = cli.add_subcommand("gen-pin", "Creates a new random password for an authority.");
    cmd->add_option("c-pin-obj", credentialObj, "The authority's credential object in C_PIN.");
    cmd->callback([&] {
        const auto credentialUid = Unwrap(FindOrParseUid(app, credentialObj, currentSP), "cannot find credential object");
        app.GenMEK(credentialUid);
    });
}


void AddCmdRevert(SEDManager& app, CLI::App& cli) {
    static std::string spName;
    auto cmd = cli.add_subcommand("revert", "Revert an SP to Original Manufacturing State. MAY ERASE DRIVE!");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&] {
        const auto spUid = Unwrap(FindOrParseUid(app, spName, currentSP), "cannot find security provider");
        app.Revert(spUid);
        ClearCurrents();
    });
}


void AddCmdActivate(SEDManager& app, CLI::App& cli) {
    static std::string spName;
    auto cmd = cli.add_subcommand("activate", "Activate an SP that's been disabled the manufacturer.");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.");
    cmd->callback([&] {
        const auto spUid = Unwrap(FindOrParseUid(app, spName, currentSP), "cannot find security provider");
        app.Activate(spUid);
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
const std::unordered_set<Uid>& GetCurrentAuthorities() { return currentAuthorities; }

} // namespace interactive