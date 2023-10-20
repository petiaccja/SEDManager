#include "TPerModules.hpp"

#include <algorithm>


void TPerModules::Load(std::shared_ptr<Module> mod) {
    for (auto& feature : mod->Features()) {
        Load(feature);
    }

    m_modules.push_back(mod);
    const auto cmpLtName = [](const auto& lhs, const auto& rhs) { return lhs->ModuleName() < rhs->ModuleName(); };
    const auto cmpEqName = [](const auto& lhs, const auto& rhs) { return lhs->ModuleName() == rhs->ModuleName(); };
    const auto cmpLtKind = [](const auto& lhs, const auto& rhs) { return lhs->ModuleKind() < rhs->ModuleKind(); };
    std::ranges::sort(m_modules, cmpLtName);
    const auto duplicates = std::ranges::unique(m_modules, cmpEqName);
    m_modules.erase(std::begin(duplicates), std::end(duplicates));

    // Sort so that lookup is tried first in Features, then in SSCs, then in Core.
    std::ranges::sort(m_modules, cmpLtKind);
}


std::optional<std::string> TPerModules::FindName(Uid uid) const {
    for (const auto& mod : m_modules) {
        const auto maybeName = mod->FindName(uid);
        if (maybeName) {
            return *maybeName;
        }
    }
    return std::nullopt;
}


std::optional<Uid> TPerModules::FindUid(std::string_view name) const {
    for (const auto& mod : m_modules) {
        const auto maybeUid = mod->FindUid(name);
        if (maybeUid) {
            return *maybeUid;
        }
    }
    return std::nullopt;
}


std::optional<TableDesc> TPerModules::FindTable(Uid table) const {
    for (const auto& mod : m_modules) {
        const auto maybeTable = mod->FindTable(table);
        if (maybeTable) {
            return *maybeTable;
        }
    }
    return std::nullopt;
}


std::optional<Type> TPerModules::FindType(Uid uid) const {
    for (const auto& mod : m_modules) {
        const auto maybeType = mod->FindType(uid);
        if (maybeType) {
            return *maybeType;
        }
    }
    return std::nullopt;
}
