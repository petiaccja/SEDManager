#pragma once

#include <Specification/Module.hpp>


namespace sedmgr {

class ModuleCollection {
public:
    void Load(std::shared_ptr<Module> mod);

    std::optional<std::string> FindName(UID uid, std::optional<UID> sp = {}) const;
    std::optional<UID> FindUid(std::string_view name, std::optional<UID> sp = {}) const;
    std::optional<TableDesc> FindTable(UID table) const;
    std::optional<Type> FindType(UID uid) const;

    auto begin() { return m_modules.begin(); }
    auto begin() const { return m_modules.begin(); }
    auto cbegin() const { return m_modules.cbegin(); }
    auto end() { return m_modules.end(); }
    auto end() const { return m_modules.end(); }
    auto cend() const { return m_modules.cend(); }

private:
    std::vector<std::shared_ptr<Module>> m_modules;
};

} // namespace sedmgr