#pragma once

#include "Common/TableDesc.hpp"

#include <Messaging/Native.hpp>
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

    virtual std::optional<std::string> FindName(UID uid, std::optional<UID> sp = std::nullopt) const;
    virtual std::optional<UID> FindUid(std::string_view name, std::optional<UID> sp = std::nullopt) const;
    virtual std::optional<TableDesc> FindTable(UID table) const;
    virtual std::optional<Type> FindType(UID uid) const;

    virtual std::span<const std::shared_ptr<Module>> Features() const;
};

} // namespace sedmgr