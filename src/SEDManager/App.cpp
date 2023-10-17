#include "App.hpp"

#include <Specification/Identifiers.hpp>
#include <Specification/Names.hpp>
#include <Specification/Tables.hpp>

#include <stdexcept>


App::App(std::string_view device) {
    m_device = std::make_shared<NvmeDevice>(device);
    m_tper = std::make_shared<TrustedPeripheral>(m_device);
    m_capabilities = m_tper->GetDesc();
    const auto comIdState = m_tper->VerifyComId();
    if (comIdState != eComIdState::ISSUED && comIdState != eComIdState::ASSOCIATED) {
        throw std::runtime_error("failed to acquire valid ComID");
    }
    m_sessionManager = std::make_shared<SessionManager>(m_tper);
}


const TPerDesc& App::GetCapabilities() const {
    return m_tper->GetDesc();
}


std::unordered_map<std::string, uint32_t> App::GetProperties() {
    const std::unordered_map<std::string, uint32_t> hostProperties = {
        {"MaxPackets",        1    },
        { "MaxSubpackets",    1    },
        { "MaxMethods",       1    },
        { "MaxComPacketSize", 65536},
        { "MaxIndTokenSize",  65536},
        { "MaxAggTokenSize",  65536},
        { "ContinuedTokens",  0    },
        { "SequenceNumbers",  0    },
        { "AckNAK",           0    },
        { "Asynchronous",     0    },
    };

    auto [tperProps, hostProps] = m_sessionManager->Properties(hostProperties);
    return tperProps;
}


std::optional<std::string> App::GetNameFromTable(Uid uid, std::optional<uint32_t> column) {
    const auto name = GetName(uid);
    if (name) {
        return std::string(*name);
    }
    else if (column) {
        try {
            auto name = value_cast<std::string>(m_session->base.Get(uid, *column));
            return { std::move(name) };
        }
        catch (std::exception&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}


std::vector<NamedObject> App::GetNamedRows(const Table& table) {
    std::vector<NamedObject> namedRows;
    for (const auto& row : table) {
        namedRows.emplace_back(row.Id(), GetNameOrUid(row.Id()));
    }
    return namedRows;
}


std::vector<NamedObject> App::GetSecurityProviders() {
    auto get = [this](std::shared_ptr<Session> session) {
        std::vector<NamedObject> securityProviders;

        Table sp = Table(eTable::SP, session);
        return GetNamedRows(sp);
    };

    if (m_session) {
        return get(m_session);
    }
    else {
        const auto session = std::make_shared<Session>(m_sessionManager, opal::eSecurityProvider::Admin);
        return get(session);
    }
}


std::vector<NamedObject> App::GetAuthorities() {
    if (m_session) {
        Table auth = Table(eTable::Authority, m_session);
        return GetNamedRows(auth);
    }
    throw std::logic_error("a session must be active");
}


std::vector<NamedObject> App::GetTables() {
    if (m_session) {
        Table table = Table(eTable::Table, m_session);
        return GetNamedRows(table);
    }
    throw std::logic_error("a session must be active");
}


Table App::GetTable(Uid table) {
    if (m_session) {
        return Table(table, m_session);
    }
    throw std::logic_error("a session must be active");
}


Object App::GetObject(Uid table, Uid object) {
    if (m_session) {
        Table t(table, m_session);
        return Object(object, t.GetDesc(), m_session);
    }
    throw std::logic_error("a session must be active");
}


Value App::Get(Uid object, uint32_t column) {
    if (m_session) {
        return m_session->base.Get(object, column);
    }
    throw std::logic_error("a session must be active");
}


void App::Set(Uid object, uint32_t column, Value value) {
    if (m_session) {
        return m_session->base.Set(object, column, value);
    }
    throw std::logic_error("a session must be active");
}


void App::Start(Uid securityProvider) {
    if (m_session) {
        m_session = {};
    }
    m_session = std::make_shared<Session>(m_sessionManager, securityProvider);
}


void App::Authenticate(Uid authority, std::optional<std::span<const std::byte>> password) {
    if (m_session) {
        return m_session->base.Authenticate(authority, password);
    }
    throw std::logic_error("start a session on an SP to authenticate");
}


void App::End() {
    m_session = {};
}


void App::StackReset() {
    m_tper->StackReset();
    End();
}


void App::Reset() {
    m_tper->Reset();
    End();
}


void App::Revert(std::span<const std::byte> psidPassword) {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin, psidPassword, opal::eAuthority::PSID);
    session.opal.Revert(opal::eSecurityProvider::Admin);
    End();
}


Table::Table(Uid table, std::shared_ptr<Session> session)
    : m_table(table), m_session(session) {
    // TODO: Get dynamic description from the 'Table' table and 'Column' table if available.
    // Looks like devices don't have proper data in these tables, hence the static description.
    try {
        m_desc = GetTableDesc(table);
    }
    catch (std::exception& ex) {
        throw std::runtime_error(std::format("not implemented: static table description missing for table '{}'", GetNameOrUid(table)));
    }

    if (m_desc.columns.empty()) {
        throw std::runtime_error(std::format("not implemented: static column descriptions missing for table '{}'", GetNameOrUid(table)));
    }
}


Uid Table::First() const {
    if (m_session) {
        if (m_desc.singleRow) {
            return *m_desc.singleRow;
        }
        return m_session->base.Next(m_table, {}).value_or(0);
    }
    return 0;
}


template <bool Mutable>
Uid Table::row_iterator<Mutable>::Next() const {
    if (m_session) {
        if (m_desc.singleRow) {
            return 0;
        }
    }
    return m_session->base.Next(m_table, m_row).value_or(0);
}