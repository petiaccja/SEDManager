#pragma once


#include "../Module.hpp"


namespace opal {

enum class eMethod : uint64_t {
    Revert = 0x0000'0006'0000'0202,
    Activate = 0x0000'0006'0000'0203,
};

}


class OpalModule : public Module {
public:
    std::string_view ModuleName() const override;
    eModuleKind ModuleKind() const override;

    std::optional<std::string> FindName(Uid uid) const override;
    std::optional<Uid> FindUid(std::string_view name) const override;
    std::optional<TableDesc> FindTable(Uid table) const override;
    std::optional<Type> FindType(Uid uid) const override;

    std::span<const std::shared_ptr<Module>> Features() const override;
};