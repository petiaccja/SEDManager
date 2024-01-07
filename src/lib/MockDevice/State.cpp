#include "State.hpp"


namespace sedmgr {

namespace mock {


    Object::Object(UID uid, std::initializer_list<Value> values)
        : m_uid(uid) {
        m_values.push_back(Value(Serialize(uid)));
        m_values.insert(m_values.end(), values);
    }


    size_t Object::Size() const {
        return m_values.size();
    }


    Value& Object::operator[](size_t column) {
        assert(column < m_values.size());
        return m_values[column];
    }


    const Value& Object::operator[](size_t column) const {
        assert(column < m_values.size());
        return m_values[column];
    }


    UID Object::GetUID() const {
        return m_uid;
    }


    Table::Table(UID uid, std::initializer_list<Object> objects)
        : m_uid(uid) {
        std::optional<size_t> numColumns;
        for (const auto& object : objects) {
            if (!numColumns) {
                numColumns = object.Size();
            }
            if (*numColumns != object.Size()) {
                throw std::invalid_argument("all object must have the same size");
            }
            m_objects.insert_or_assign(object.GetUID(), object);
        }
    }


    bool Table::contains(UID object) const {
        return m_objects.contains(object);
    }


    std::unordered_map<UID, Object>::iterator Table::find(UID object) {
        return m_objects.find(object);
    }


    std::unordered_map<UID, Object>::const_iterator Table::find(UID object) const {
        return m_objects.find(object);
    }


    std::unordered_map<UID, Object>::iterator Table::begin() {
        return m_objects.begin();
    }


    std::unordered_map<UID, Object>::const_iterator Table::begin() const {
        return m_objects.begin();
    }


    std::unordered_map<UID, Object>::iterator Table::end() {
        return m_objects.end();
    }


    std::unordered_map<UID, Object>::const_iterator Table::end() const {
        return m_objects.end();
    }


    Object& Table::operator[](UID object) {
        const auto it = m_objects.find(object);
        assert(it != m_objects.end());
        return it->second;
    }


    const Object& Table::operator[](UID object) const {
        const auto it = m_objects.find(object);
        assert(it != m_objects.end());
        return it->second;
    }


    UID Table::GetUID() const {
        return m_uid;
    }


    SecurityProvider::SecurityProvider(UID uid, std::initializer_list<Table> tables)
        : m_uid(uid) {
        for (const auto& table : tables) {
            m_tables.insert_or_assign(table.GetUID(), table);
        }
    }


    bool SecurityProvider::contains(UID table) const {
        return m_tables.contains(table);
    }


    Table& SecurityProvider::operator[](UID table) {
        const auto it = m_tables.find(table);
        assert(it != m_tables.end());
        return it->second;
    }


    const Table& SecurityProvider::operator[](UID table) const {
        const auto it = m_tables.find(table);
        assert(it != m_tables.end());
        return it->second;
    }


    UID SecurityProvider::GetUID() const {
        return m_uid;
    }

} // namespace mock

} // namespace sedmgr