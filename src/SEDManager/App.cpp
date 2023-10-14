#include "App.hpp"

#include <Specification/Authorities.hpp>
#include <Specification/Names.hpp>
#include <Specification/SecurityProviders.hpp>
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


std::vector<NamedObject> App::GetNamedRows(Session& session, const Table& table, uint32_t nameColumn) {
    std::vector<NamedObject> namedRows;
    for (const auto& row : table) {
        std::optional<std::string> name = GetNameFromTable(row.Id(), uint32_t(eColumns_SP::Name));
        namedRows.emplace_back(row.Id(), std::move(name));
    }
    return namedRows;
}


std::vector<NamedObject> App::GetSecurityProviders() {
    auto get = [this](std::shared_ptr<Session> session) {
        std::vector<NamedObject> securityProviders;

        Table sp = Table(eTable::SP, session);
        return GetNamedRows(*session, sp, uint32_t(eColumns_SP::Name));
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
        return GetNamedRows(*m_session, auth, uint32_t(eColumns_Authority::Name));
    }
    throw std::logic_error("a session must be active");
}


std::vector<NamedObject> App::GetTables() {
    if (m_session) {
        Table table = Table(eTable::Table, m_session);
        std::vector<NamedObject> namedRows;
        for (const auto& row : table) {
            const auto tableId = DescriptorToTable(row.Id());
            std::optional<std::string> name = GetNameFromTable(tableId, uint32_t(eColumns_SP::Name));
            namedRows.emplace_back(tableId, std::move(name));
        }
        return namedRows;
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
        return Object(object, t.NumColumns(), m_session);
    }
    throw std::logic_error("start a session to query tables");
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
    Uid descriptor = TableToDescriptor(table);
    const auto descIt = tables::table.find(descriptor);
    if (descIt != tables::table.end() && descIt->second.numColumns != 0) {
        m_numColumns = descIt->second.numColumns;
    }
    else {
        try {
            m_numColumns = m_session->base.Get(descriptor, 6).Get<size_t>();
        }
        catch (std::exception& ex) {
            throw std::runtime_error(std::format("could not resolve number of columns: TPer has no information: {}", ex.what()));
        }
    }
}


Uid Table::First() const {
    if (m_session) {
        const auto descIt = tables::table.find(m_table);
        if (descIt != tables::table.end()) {
            if (descIt->second.singleRow) {
                return descIt->second.singleRow.value();
            }
        }
        return m_session->base.Next(m_table, {}).value_or(0);
    }
    return 0;
}


template <bool Mutable>
Uid Table::row_iterator<Mutable>::Next() const {
    const auto descIt = tables::table.find(m_table);
    if (descIt != tables::table.end()) {
        return 0;
    }
    return m_session->base.Next(m_table, m_row).value_or(0);
}