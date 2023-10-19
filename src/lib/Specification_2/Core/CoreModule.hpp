#pragma once


#include "../Module.hpp"

#include "Data/UIDs.hpp" // So that users don't have to include files directly from CoreModule's implementation.


class CoreModule : public Module {
public:
    std::string_view ModuleName() const override;
    eModuleKind ModuleKind() const override;

    std::optional<std::string> FindName(Uid uid) const override;
    std::optional<Uid> FindUid(std::string_view name) const override;
    std::optional<TableDesc> FindTable(Uid table) const override;
    std::optional<Type> FindType(Uid uid) const override;
};