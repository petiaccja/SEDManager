#pragma once

#include "Common/TableDesc.hpp"

#include <Data/NativeTypes.hpp>
#include <Data/Type.hpp>

#include <memory>
#include <optional>
#include <string_view>



enum class eModuleKind {
    CORE,
    SSC,
    FEATURE,
};


class Module {
public:
    virtual ~Module() = default;

    virtual std::string_view ModuleName() const = 0;
    virtual eModuleKind ModuleKind() const = 0;

    virtual std::optional<std::string> FindName(Uid uid) const;
    virtual std::optional<Uid> FindUid(std::string_view name) const;
    virtual std::optional<TableDesc> FindTable(Uid table) const;
    virtual std::optional<Type> FindType(Uid uid) const;

    virtual std::span<const std::shared_ptr<Module>> Features() const;
};