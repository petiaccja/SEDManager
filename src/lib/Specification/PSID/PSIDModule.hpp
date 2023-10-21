#pragma once

#include "../Module.hpp"


class PSIDModule : public Module {
public:
    static std::shared_ptr<Module> Get();

    std::string_view ModuleName() const override;
    eModuleKind ModuleKind() const override;

    std::optional<std::string> FindName(Uid uid, std::optional<Uid> = std::nullopt) const override;
    std::optional<Uid> FindUid(std::string_view name, std::optional<Uid> = std::nullopt) const override;
};