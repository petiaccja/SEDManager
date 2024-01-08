#pragma once


#include "../Module.hpp"


namespace sedmgr {

namespace opal {

    enum class eMethod : uint64_t {
        Revert = 0x0000'0006'0000'0202,
        Activate = 0x0000'0006'0000'0203,
    };

}


class OpalModule : public Module {
public:
    eModuleKind ModuleKind() const override;

    std::optional<std::string> FindName(UID uid, std::optional<UID> sp = {}) const override;
    std::optional<UID> FindUid(std::string_view name, std::optional<UID> sp = {}) const override;
    std::optional<TableDesc> FindTable(UID table) const override;
    std::optional<Type> FindType(UID uid) const override;

    std::span<const std::shared_ptr<Module>> Features() const override;
};


class Opal1Module : public OpalModule {
public:
    static std::shared_ptr<Module> Get();
    std::string_view ModuleName() const override;
};


class Opal2Module : public OpalModule {
public:
    static std::shared_ptr<Module> Get();
    std::string_view ModuleName() const override;
};

} // namespace sedmgr