#pragma once

#include "../Module.hpp"


class PSIDModule : public Module {
public:
    static std::shared_ptr<Module> Get();

    std::string_view ModuleName() const override;
    eModuleKind ModuleKind() const override;

    std::optional<std::string> FindName(Uid uid) const override;
    std::optional<Uid> FindUid(std::string_view name) const override;
};