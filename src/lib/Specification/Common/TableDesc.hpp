#pragma once

#include <Data/NativeTypes.hpp>
#include <Data/Type.hpp>


enum class eTableKind {
    OBJECT,
    BYTE,
};


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