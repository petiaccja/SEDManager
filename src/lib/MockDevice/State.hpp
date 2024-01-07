#pragma once

#include <Messaging/NativeTypes.hpp>
#include <Messaging/Value.hpp>
#include <TrustedPeripheral/MethodUtils.hpp>

#include <unordered_map>
#include <unordered_set>


namespace sedmgr {

namespace mock {

    class Object {
    public:
        Object(UID uid, std::initializer_list<Value> values);

        size_t Size() const;
        Value& operator[](size_t column);
        const Value& operator[](size_t column) const;
        UID GetUID() const;

    private:
        UID m_uid;
        std::vector<Value> m_values;
    };


    class Table {
        using ObjectMap = std::unordered_map<UID, Object>;

    public:
        Table(UID uid, std::initializer_list<Object> objects);

        bool contains(UID object) const;
        ObjectMap::iterator find(UID object);
        ObjectMap::const_iterator find(UID object) const;
        ObjectMap::iterator begin();
        ObjectMap::const_iterator begin() const;
        ObjectMap::iterator end();
        ObjectMap::const_iterator end() const;
        Object& operator[](UID object);
        const Object& operator[](UID object) const;
        UID GetUID() const;

    private:
        UID m_uid;
        ObjectMap m_objects;
    };


    class SecurityProvider {
    public:
        SecurityProvider(UID uid, std::initializer_list<Table> tables);

        bool contains(UID table) const;
        Table& operator[](UID table);
        const Table& operator[](UID table) const;
        UID GetUID() const;

    private:
        UID m_uid;
        std::unordered_map<UID, Table> m_tables;
    };

} // namespace mock

} // namespace sedmgr