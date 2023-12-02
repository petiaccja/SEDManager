#pragma once

#include <Data/NativeTypes.hpp>
#include <Data/Value.hpp>
#include <TrustedPeripheral/Method.hpp>

#include <unordered_map>
#include <unordered_set>


namespace sedmgr {


template <class T>
using Expected = std::variant<std::conditional_t<std::is_void_v<T>, std::monostate, T>, eMethodStatus>;


class MockObject {
public:
    MockObject(Uid uid, std::initializer_list<Value> values);

    Expected<void> Set(const std::unordered_map<uint32_t, Value>& values);
    Expected<std::unordered_map<uint32_t, Value>> Get(uint32_t firstColumn, uint32_t lastColumn) const;
    size_t Size() const;
    Uid GetUID() const;

private:
    const Uid m_uid;
    std::vector<Value> m_values;
};


class MockTable {
public:
    MockTable(Uid uid, std::initializer_list<Uid> objects);
    Expected<Uid> Next(Uid from) const;
    Uid GetUID() const;

private:
    const Uid m_uid;
    std::unordered_set<Uid> m_objects;
};


class MockSecurityProvider {
public:
    MockSecurityProvider(Uid uid, std::initializer_list<MockTable> tables, std::initializer_list<MockObject> objects);

    Expected<std::reference_wrapper<MockTable>> GetTable(Uid uid);
    Expected<std::reference_wrapper<const MockTable>> GetTable(Uid uid) const;
    Expected<std::reference_wrapper<MockObject>> GetObject(Uid uid);
    Expected<std::reference_wrapper<const MockObject>> GetObject(Uid uid) const;
    Uid GetUID() const;

private:
    Uid m_uid;
    std::unordered_map<Uid, MockTable> m_tables;
    std::unordered_map<Uid, MockObject> m_objects;
};

} // namespace sedmgr