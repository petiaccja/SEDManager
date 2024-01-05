#pragma once

#include "Common/TableDesc.hpp"

#include <Messaging/NativeTypes.hpp>
#include <Messaging/Type.hpp>

#include <memory>
#include <optional>
#include <string_view>


namespace sedmgr {

// Values determine the order in which lookup happens in case of multiple modules.
// We want features first, then SSCs, and finally core.
enum class eModuleKind {
    FEATURE = 1,
    SSC = 2,
    CORE = 3,
};


class Module {
public:
    virtual ~Module() = default;

    virtual std::string_view ModuleName() const = 0;
    virtual eModuleKind ModuleKind() const = 0;

    virtual std::optional<std::string> FindName(Uid uid, std::optional<Uid> sp = std::nullopt) const;
    virtual std::optional<Uid> FindUid(std::string_view name, std::optional<Uid> sp = std::nullopt) const;
    virtual std::optional<TableDesc> FindTable(Uid table) const;
    virtual std::optional<Type> FindType(Uid uid) const;

    virtual std::span<const std::shared_ptr<Module>> Features() const;
};

} // namespace sedmgr