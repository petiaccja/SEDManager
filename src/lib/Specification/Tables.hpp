#pragma once

#include "Enumerations.hpp"

#include <Data/NativeTypes.hpp>
#include <Data/Type.hpp>

#include <cstdint>
#include <vector>


constexpr Uid TableToDescriptor(Uid table) {
    return (uint64_t(table) >> 32) | (1ull << 32);
}


constexpr Uid DescriptorToTable(Uid descriptor) {
    return uint64_t(descriptor) << 32;
}


struct ColumnDesc {
    std::string name;
    bool isUnique;
    Type type;
};


struct TableDesc {
    std::string name;
    eTableKind kind;
    std::vector<ColumnDesc> columns = {};
    std::optional<Uid> singleRow = std::nullopt;
};


TableDesc GetTableDesc(Uid table);