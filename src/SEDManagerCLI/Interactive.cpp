#include "Interactive.hpp"

#include "Utility.hpp"

#include <Archive/Types/ValueToJSON.hpp>

#include <CLI/App.hpp>
#include <SEDManager/SEDManager.hpp>

#include <iostream>
#include <ostream>


// DO NOT REORDER THIS. Includes <Windows.h>
// clang-format: off
#include <rang.hpp>
// clang-format: on

// I swear to god this is the second most retarded API I've seen after OpenGL.
#ifdef GetObject
    #undef GetObject
#endif


Interactive::Interactive(SEDManager& manager) : m_manager(manager) {
    m_cli.set_help_flag();
    m_cli.set_help_all_flag();
    m_cli.set_version_flag();

    RegisterCallbackExit();
    RegisterCallbackHelp();

    RegisterCallbackStart();
    RegisterCallbackAuthenticate();
    RegisterCallbackEnd();

    RegisterCallbackInfo();
    RegisterCallbackFind();
    RegisterCallbackRows();
    RegisterCallbackColumns();
    RegisterCallbackGet();
    RegisterCallbackSet();
    RegisterCallbackPasswd();
    RegisterCallbackGenMEK();
    RegisterCallbackGenPIN();
    RegisterCallbackActivate();
    RegisterCallbackRevert();

    RegisterCallbackStackReset();
    RegisterCallbackReset();
}


std::optional<Uid> Interactive::GetCurrentSecurityProvider() const {
    return m_currentSecurityProvider;
}


std::unordered_set<Uid> Interactive::GetCurrentAuthorities() const {
    return m_currentAuthorities;
}


int Interactive::Run() {
    std::string command;
    while (!m_finished && std::cin.good()) {
        try {
            PrintCaret();
            std::getline(std::cin, command);
            m_cli.parse(command, false);
        }
        catch (CLI::CallForHelp&) {
            PrintHelp(command);
        }
        catch (std::exception& ex) {
            std::cout << rang::fg::red << "Error: " << rang::style::reset << ex.what() << std::endl;
        }
    }
    return 0;
}


void Interactive::ClearCurrents() {
    m_currentSecurityProvider = std::nullopt;
    m_currentAuthorities.clear();
}


auto Interactive::ParseGetSet(std::string rowName, int32_t column) const -> std::optional<std::tuple<Uid, Uid, int32_t>> {
    const auto& rowNameSections = SplitName(rowName);
    if (rowNameSections.size() != 2) {
        throw std::invalid_argument("specify object as 'Table::Object'");
    }

    const auto tableUid = Unwrap(FindOrParseUid(m_manager, rowNameSections[0], m_currentSecurityProvider), "cannot find table");
    const auto maybeRowUid = FindOrParseUid(m_manager, rowName, m_currentSecurityProvider).value_or(FindOrParseUid(m_manager, rowNameSections[1], m_currentSecurityProvider).value_or(0));
    if (maybeRowUid == Uid(0)) {
        throw std::invalid_argument("cannot find object");
    }
    auto copy = column;
    column = -1;
    return std::tuple{ tableUid, maybeRowUid, copy };
};


void Interactive::PrintCaret() const {
    std::string currentSPName = "<no session>";
    std::vector<std::string> currentAuthNames;
    const auto currentSP = GetCurrentSecurityProvider();
    if (currentSP) {
        currentSPName = m_manager.GetModules().FindName(*currentSP).value_or(to_string(*currentSP));
        for (auto auth : GetCurrentAuthorities()) {
            const std::string n = m_manager.GetModules().FindName(auth, *currentSP).value_or(to_string(auth));
            currentAuthNames.push_back(std::string(SplitName(n).back()));
        }
    }

    std::cout << rang::fg::cyan << SplitName(currentSPName).back() << rang::style::reset;
    if (currentSP) {
        std::cout << "[ ";
        if (currentAuthNames.empty()) {
            std::cout << rang::fg::green << "Anybody" << rang::style::reset;
        }
        else {
            std::cout << rang::fg::green << Join(currentAuthNames, ", ") << rang::style::reset;
        }
        std::cout << " ]";
    }
    std::cout << "> ";
}


void Interactive::PrintHelp(const std::string& command) const {
    const auto tokens = CLI::detail::split_up(command);
    assert(!tokens.empty()); // "help" must be the first token if we got here.
    auto subcommand = &m_cli;
    try {
        for (auto& token : tokens | std::views::drop(1)) {
            subcommand = subcommand->get_subcommand(token);
        }
    }
    catch (...) {
        // Empty
    }
    std::cout << subcommand->help() << std::endl;
}


void Interactive::RegisterCallbackExit() {
    auto cmdExit = m_cli.add_subcommand("exit", "Exit the application.");
    cmdExit->callback([&] {
        m_finished = true;
    });
}


void Interactive::RegisterCallbackHelp() {
    auto cmd = m_cli.add_subcommand("help", "Print this help message.")->silent()->allow_extras();
    cmd->parse_complete_callback([] {
        throw CLI::CallForHelp();
    });
}


void Interactive::RegisterCallbackStart() {
    static std::string spName;

    auto cmd = m_cli.add_subcommand("start", "Start a session with a service provider.");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.")->required();
    cmd->callback([this] {
        const auto spUid = Unwrap(FindOrParseUid(m_manager, "SP::" + spName, m_currentSecurityProvider), "cannot find security provider");
        m_manager.Start(spUid);
        m_currentSecurityProvider = spUid;
    });
}


void Interactive::RegisterCallbackAuthenticate() {
    static std::string authName;

    auto cmd = m_cli.add_subcommand("auth", "Authenticate with an authority.");
    cmd->add_option("authority", authName, "The name or UID (in hex) of the security provider.")->required();
    cmd->callback([this] {
        const auto authUid = Unwrap(FindOrParseUid(m_manager, "Authority::" + authName, m_currentSecurityProvider), "cannot find authority");
        const auto password = GetPassword("Password: ");
        m_manager.Authenticate(authUid, password);
        m_currentAuthorities.insert(authUid);
    });
}


void Interactive::RegisterCallbackEnd() {
    auto cmd = m_cli.add_subcommand("end", "End session with current service provider.");
    cmd->callback([this] {
        m_manager.End();
        ClearCurrents();
    });
}


void Interactive::RegisterCallbackInfo() {
    auto cmd = m_cli.add_subcommand("info", "Print TCG support information.");
    const auto verbose = cmd->add_flag("-v,--verbose", "Print extended information about the device.");
    cmd->callback([this, verbose] {
        const auto& desc = m_manager.GetDesc();
        const auto& mods = m_manager.GetModules();
        std::vector<std::string_view> sscNames;
        for (auto& sscDesc : desc.sscDescs) {
            sscNames.push_back(std::visit([](auto& d) { return d.featureName; }, sscDesc));
        }
        std::vector<std::string_view> modNames;
        for (auto& mod : mods) {
            modNames.push_back(mod->ModuleName());
        }
        std::cout << "TCG storage subsystem classes: " << Join(sscNames, ", ") << std::endl;
        std::cout << "Modules loaded: " << Join(modNames, ", ") << std::endl;

        if (*verbose) {
            std::vector<std::string> columns = { "Parameter", "Value" };
            std::vector<std::vector<std::string>> rows;
            if (desc.tperDesc) {
                rows = {
                    {"ComID management supported",   desc.tperDesc->comIdMgmtSupported ? "yes" : "no" },
                    { "Streaming supported",         desc.tperDesc->streamingSupported ? "yes" : "no" },
                    { "Buffer management supported", desc.tperDesc->bufferMgmtSupported ? "yes" : "no"},
                    { "ACK/NAK supported",           desc.tperDesc->ackNakSupported ? "yes" : "no"    },
                    { "Async supported",             desc.tperDesc->asyncSupported ? "yes" : "no"     },
                    { "Sync supported",              desc.tperDesc->syncSupported ? "yes" : "no"      },
                };
                std::cout << "\nTPer description:" << std::endl;
                std::cout << FormatTable(columns, rows);
            }
            if (desc.lockingDesc) {
                rows = {
                    {"Shadow MBR supported",        desc.lockingDesc->mbrSupported ? "yes" : "no"    },
                    { "Shadow MBR done",            desc.lockingDesc->mbrDone ? "yes" : "no"         },
                    { "Shadow MBR enabled",         desc.lockingDesc->mbrEnabled ? "yes" : "no"      },
                    { "Media encryption supported", desc.lockingDesc->mediaEncryption ? "yes" : "no" },
                    { "Locked",                     desc.lockingDesc->locked ? "yes" : "no"          },
                    { "Locking enabled",            desc.lockingDesc->lockingEnabled ? "yes" : "no"  },
                    { "Locking supported",          desc.lockingDesc->lockingSupported ? "yes" : "no"},
                };
                std::cout << "\nLocking description:" << std::endl;
                std::cout << FormatTable(columns, rows);
            }
            for (const auto& sscDesc : desc.sscDescs) {
                const auto visitor = [&](auto& d) {
                    rows = {
                        {"Base ComID",     std::to_string(d.baseComId)},
                        { "Nr. of ComIDs", std::to_string(d.numComIds)},
                    };
                    if constexpr (requires { d.crossingRangeBehavior; }) {
                        rows.push_back({ "Crossing range behavior", std::to_string(d.crossingRangeBehavior) });
                    }
                    if constexpr (requires { d.numAdminsSupported; }) {
                        rows.push_back({ "Nr. of Admins supported", std::to_string(d.numAdminsSupported) });
                    }
                    if constexpr (requires { d.numUsersSupported; }) {
                        rows.push_back({ "Nr. of Users supported", std::to_string(d.numUsersSupported) });
                    }
                    if constexpr (requires { d.initialCPinSidIndicator; }) {
                        rows.push_back({ "Initial C_PIN::SID indicator", std::to_string(d.initialCPinSidIndicator) });
                    }
                    if constexpr (requires { d.cPinSidRevertBehavior; }) {
                        rows.push_back({ "C_PIN::SID Revert behavior", std::to_string(d.cPinSidRevertBehavior) });
                    }
                    std::cout << "\n"
                              << d.featureName << " feature description:" << std::endl;
                    std::cout << FormatTable(columns, rows);
                };
                std::visit(visitor, sscDesc);
            }
        }
    });
}


void Interactive::RegisterCallbackFind() {
    static std::string objectName;
    auto cmd = m_cli.add_subcommand("find", "Finds the name and UID of an object given as name or UID.");
    cmd->add_option("object", objectName)->required();
    cmd->callback([this] {
        const auto objectUid = Unwrap(FindOrParseUid(m_manager, objectName, m_currentSecurityProvider), "cannot find object");
        const auto maybeName = m_manager.GetModules().FindName(objectUid, m_currentSecurityProvider);
        std::cout << "UID:  " << to_string(objectUid) << std::endl;
        std::cout << "Name: " << maybeName.value_or("<not found>") << std::endl;
    });
}


void Interactive::RegisterCallbackRows() {
    static std::string tableName;

    auto cmdRows = m_cli.add_subcommand("rows", "List the rows of the table.");
    cmdRows->add_option("table", tableName, "The table to list the rows of.")->required();
    cmdRows->callback([this] {
        const auto tableUid = Unwrap(FindOrParseUid(m_manager, tableName, m_currentSecurityProvider), "cannot find table");
        const auto table = m_manager.GetTable(tableUid);

        const std::vector<std::string> columnNames = { "UID", "Name" };
        std::vector<std::vector<std::string>> rows;
        for (const auto& row : table) {
            rows.push_back({ to_string(row.Id()), m_manager.GetModules().FindName(row.Id(), m_currentSecurityProvider).value_or("") });
        }
        std::cout << FormatTable(columnNames, rows);
    });
}


void Interactive::RegisterCallbackColumns() {
    static std::string tableName;

    auto cmdColumns = m_cli.add_subcommand("columns", "List the columns of the table.");
    cmdColumns->add_option("table", tableName, "The table to list the columns of.")->required();
    cmdColumns->callback([this] {
        const auto tableUid = Unwrap(FindOrParseUid(m_manager, tableName, m_currentSecurityProvider), "cannot find table");
        const auto table = m_manager.GetTable(tableUid);

        size_t columnNumber = 0;
        const std::vector<std::string> columnNames = { "Number", "Name", "IsUnique", "Type" };
        std::vector<std::vector<std::string>> rows;
        for (const auto& column : table.GetDesc().columns) {
            rows.push_back({ std::to_string(columnNumber++), column.name, column.isUnique ? "yes" : "", GetTypeStr(column.type) });
        }
        std::cout << FormatTable(columnNames, rows);
    });
}


void Interactive::RegisterCallbackGet() {
    static std::string rowName;
    static int32_t column = -1;
    static std::string jsonValue;

    auto cmdGet = m_cli.add_subcommand("get", "Get a cell from a table.");

    cmdGet->add_option("object", rowName, "The object to get the cells of. Format as 'Table::Object'.")->required();
    cmdGet->add_option("column", column, "The column to get.")->default_val(-1);
    cmdGet->callback([this] {
        const auto parsed = ParseGetSet(rowName, column);
        column = -1;
        if (!parsed) {
            return;
        }
        const auto& [tableUid, rowUid, column] = *parsed;
        const auto object = m_manager.GetObject(tableUid, rowUid);
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
}


void Interactive::RegisterCallbackSet() {
    static std::string rowName;
    static int32_t column = 0;
    static std::string jsonValue;

    auto cmdSet = m_cli.add_subcommand("set", "Set a cell in a table to new value.");

    cmdSet->add_option("object", rowName, "The object to set the cells of. Format as 'Table::Object'.")->required();
    cmdSet->add_option("column", column, "The column to set.")->required();
    cmdSet->add_option("value", jsonValue, "The new value of the cell.")->required();
    cmdSet->callback([this] {
        const auto parsed = ParseGetSet(rowName, column);
        if (!parsed) {
            return;
        }
        const auto [tableUid, rowUid, column] = *parsed;
        auto object = m_manager.GetObject(tableUid, rowUid);
        const auto value = JSONToValue(nlohmann::json::parse(jsonValue), object.GetDesc()[column].type);
        object[column] = value;
    });
}


void Interactive::RegisterCallbackPasswd() {
    static std::string authName;

    auto cmd = m_cli.add_subcommand("passwd", "Change the password of an authority.");
    cmd->add_option("authority", authName, "The name or UID (in hex) of the authority.")->required();
    cmd->callback([this] {
        const auto authTable = Unwrap(FindOrParseUid(m_manager, "Authority", m_currentSecurityProvider), "cannot find Authority table");
        const auto cPinTable = Unwrap(FindOrParseUid(m_manager, "C_PIN", m_currentSecurityProvider), "cannot find C_PIN table");
        const auto authUid = Unwrap(FindOrParseUid(m_manager, "Authority::" + authName, m_currentSecurityProvider), "cannot find authority");
        const auto authority = m_manager.GetObject(authTable, authUid);
        const auto credentialUid = value_cast<Uid>(*authority[10]);
        auto credential = m_manager.GetObject(cPinTable, credentialUid);
        const std::vector<std::byte> password = GetPassword("New password: ");
        const std::vector<std::byte> passwordAgain = GetPassword("Retype password: ");
        if (password != passwordAgain) {
            throw std::invalid_argument("the two passwords do not match");
        }
        credential[3] = value_cast(password);
    });
}


void Interactive::RegisterCallbackGenMEK() {
    static std::string rangeName;
    auto cmd = m_cli.add_subcommand("gen-mek", "Creates a new Media Encryption Key for a locking range. ERASES RANGE!");
    cmd->add_option("range", rangeName, "The locking range.")->required();
    cmd->callback([&] {
        const auto rangeUid = Unwrap(FindOrParseUid(m_manager, rangeName, m_currentSecurityProvider), "cannot find locking range");
        m_manager.GenMEK(rangeUid);
    });
}


void Interactive::RegisterCallbackGenPIN() {
    static std::string credentialObj;
    static uint32_t length = 32;
    auto cmd = m_cli.add_subcommand("gen-pin", "Creates a new random password for an authority.");
    cmd->add_option("c-pin-obj", credentialObj, "The authority's credential object in C_PIN.")->required();
    cmd->callback([&] {
        const auto credentialUid = Unwrap(FindOrParseUid(m_manager, credentialObj, m_currentSecurityProvider), "cannot find credential object");
        m_manager.GenMEK(credentialUid);
    });
}


void Interactive::RegisterCallbackActivate() {
    static std::string spName;
    auto cmd = m_cli.add_subcommand("activate", "Activate an SP that's been disabled the manufacturer.");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.")->required();
    cmd->callback([&] {
        const auto spUid = Unwrap(FindOrParseUid(m_manager, spName, m_currentSecurityProvider), "cannot find security provider");
        m_manager.Activate(spUid);
    });
}


void Interactive::RegisterCallbackRevert() {
    static std::string spName;
    auto cmd = m_cli.add_subcommand("revert", "Revert an SP to Original Manufacturing State. MAY ERASE DRIVE!");
    cmd->add_option("sp", spName, "The name or UID (in hex) of the security provider.")->required();
    cmd->callback([&] {
        const auto spUid = Unwrap(FindOrParseUid(m_manager, spName, m_currentSecurityProvider), "cannot find security provider");
        m_manager.Revert(spUid);
        ClearCurrents();
    });
}


void Interactive::RegisterCallbackStackReset() {
    auto cmd = m_cli.add_subcommand("stack-reset", "Reset the current communication stream.");
    cmd->callback([this] {
        m_manager.StackReset();
        ClearCurrents();
    });
}


void Interactive::RegisterCallbackReset() {
    auto cmd = m_cli.add_subcommand("reset", "Reset the device as if power-cycled.");
    cmd->callback([this] {
        m_manager.Reset();
        ClearCurrents();
    });
}