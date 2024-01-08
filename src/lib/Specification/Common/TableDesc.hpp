#pragma once

#include <Messaging/Native.hpp>
#include <Messaging/Type.hpp>


namespace sedmgr {

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
    std::optional<UID> singleRow = std::nullopt;
};

} // namespace sedmgr