#pragma once

#include "../Module.hpp"


namespace sedmgr {

class PSIDModule : public Module {
public:
    static std::shared_ptr<Module> Get();

    std::string_view ModuleName() const override;
    eModuleKind ModuleKind() const override;

    std::optional<std::string> FindName(UID uid, std::optional<UID> = std::nullopt) const override;
    std::optional<UID> FindUid(std::string_view name, std::optional<UID> = std::nullopt) const override;
};

} // namespace sedmgr