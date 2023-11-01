#pragma once


#include "../Module.hpp"
#include "Defs/UIDs.hpp" // So that users don't have to include files directly from CoreModule's implementation.


namespace sedmgr {

class CoreModule : public Module {
public:
    static std::shared_ptr<Module> Get();

    std::string_view ModuleName() const override;
    eModuleKind ModuleKind() const override;

    std::optional<std::string> FindName(Uid uid, std::optional<Uid> = std::nullopt) const override;
    std::optional<Uid> FindUid(std::string_view name, std::optional<Uid> = std::nullopt) const override;
    std::optional<TableDesc> FindTable(Uid table) const override;
    std::optional<Type> FindType(Uid uid) const override;
};

} // namespace sedmgr