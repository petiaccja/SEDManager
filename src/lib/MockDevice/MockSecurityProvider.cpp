#include "MockSecurityProvider.hpp"


namespace sedmgr {

MockObject::MockObject(Uid uid, std::initializer_list<Value> values) : m_uid(uid) {
    m_values.push_back(value_cast(uid));
    std::ranges::copy(values, std::back_inserter(m_values));
}


Expected<void> MockObject::Set(const std::unordered_map<uint32_t, Value>& values) {
    for (auto& column : values | std::views::keys) {
        if (column == 0) {
            return eMethodStatus::NOT_AUTHORIZED;
        }
        if (column >= m_values.size()) {
            return eMethodStatus::INVALID_PARAMETER;
        }
    }
    for (auto& [column, value] : values) {
        m_values[column] = value;
    }
    return std::monostate{};
}

Expected<std::unordered_map<uint32_t, Value>> MockObject::Get(uint32_t firstColumn, uint32_t lastColumn) const {
    if (0 <= firstColumn && firstColumn <= lastColumn && lastColumn < m_values.size()) {
        std::unordered_map<uint32_t, Value> results;
        for (auto i = firstColumn; i <= lastColumn; ++i) {
            if (m_values[i].HasValue()) {
                results[i] = m_values[i];
            }
        }
        return { results };
    }
    return eMethodStatus::INVALID_PARAMETER;
}


size_t MockObject::Size() const {
    return m_values.size();
}


Uid MockObject::GetUID() const {
    return m_uid;
}


MockTable::MockTable(Uid uid, std::initializer_list<Uid> objects)
    : m_uid(uid), m_objects(objects) {}


Expected<Uid> MockTable::Next(Uid from) const {
    if (m_objects.empty()) {
        return eMethodStatus::FAIL;
    }
    if (from == Uid(0)) {
        return *m_objects.begin();
    }
    auto it = m_objects.find(from);
    if (it == m_objects.end()) {
        return eMethodStatus::INVALID_PARAMETER;
    }
    ++it;
    return it != m_objects.end() ? *it : Uid(0);
}


Uid MockTable::GetUID() const {
    return m_uid;
}


MockSecurityProvider::MockSecurityProvider(Uid uid, std::initializer_list<MockTable> tables, std::initializer_list<MockObject> objects)
    : m_uid(uid) {
    for (auto& table : tables) {
        m_tables.insert_or_assign(table.GetUID(), table);
    }
    for (auto& object : objects) {
        m_objects.insert_or_assign(object.GetUID(), object);
    }
}

Expected<std::reference_wrapper<MockTable>> MockSecurityProvider::GetTable(Uid uid) {
    const auto it = m_tables.find(uid);
    if (it != m_tables.end()) {
        return it->second;
    }
    return eMethodStatus::INVALID_PARAMETER;
}


Expected<std::reference_wrapper<const MockTable>> MockSecurityProvider::GetTable(Uid uid) const {
    const auto it = m_tables.find(uid);
    if (it != m_tables.end()) {
        return it->second;
    }
    return eMethodStatus::INVALID_PARAMETER;
}


Expected<std::reference_wrapper<MockObject>> MockSecurityProvider::GetObject(Uid uid) {
    const auto it = m_objects.find(uid);
    if (it != m_objects.end()) {
        return it->second;
    }
    return eMethodStatus::INVALID_PARAMETER;
}


Expected<std::reference_wrapper<const MockObject>> MockSecurityProvider::GetObject(Uid uid) const {
    const auto it = m_objects.find(uid);
    if (it != m_objects.end()) {
        return it->second;
    }
    return eMethodStatus::INVALID_PARAMETER;
}


Uid MockSecurityProvider::GetUID() const {
    return m_uid;
}

} // namespace sedmgr