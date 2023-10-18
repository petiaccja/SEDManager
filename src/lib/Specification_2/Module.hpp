#pragma once

#include <Data/NativeTypes.hpp>
#include <Data/Type.hpp>

#include <optional>
#include <string_view>
#include <vector>


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


class Module {
public:
    virtual ~Module() = default;
    virtual std::optional<std::string> FindName(Uid uid) const = 0;
    virtual std::optional<Uid> FindUid(std::string_view name) const = 0;
    virtual std::optional<TableDesc> FindTable(Uid table) const = 0;
    virtual std::optional<Type> FindType(Uid uid) const = 0;
};