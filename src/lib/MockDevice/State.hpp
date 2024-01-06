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
        Object(Uid uid, std::initializer_list<Value> values);

        size_t Size() const;
        Value& operator[](size_t column);
        const Value& operator[](size_t column) const;
        Uid GetUID() const;

    private:
        Uid m_uid;
        std::vector<Value> m_values;
    };


    class Table {
        using ObjectMap = std::unordered_map<Uid, Object>;

    public:
        Table(Uid uid, std::initializer_list<Object> objects);

        bool contains(Uid object) const;
        ObjectMap::iterator find(Uid object);
        ObjectMap::const_iterator find(Uid object) const;
        ObjectMap::iterator begin();
        ObjectMap::const_iterator begin() const;
        ObjectMap::iterator end();
        ObjectMap::const_iterator end() const;
        Object& operator[](Uid object);
        const Object& operator[](Uid object) const;
        Uid GetUID() const;

    private:
        Uid m_uid;
        ObjectMap m_objects;
    };


    class SecurityProvider {
    public:
        SecurityProvider(Uid uid, std::initializer_list<Table> tables);

        bool contains(Uid table) const;
        Table& operator[](Uid table);
        const Table& operator[](Uid table) const;
        Uid GetUID() const;

    private:
        Uid m_uid;
        std::unordered_map<Uid, Table> m_tables;
    };

} // namespace mock

} // namespace sedmgr