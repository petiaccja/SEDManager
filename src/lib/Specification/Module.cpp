#include "Module.hpp"


namespace sedmgr {

std::span<const std::shared_ptr<Module>> Module::Features() const {
    return {};
}


std::optional<std::string> Module::FindName(UID uid, std::optional<UID> sp) const {
    return std::nullopt;
}


std::optional<UID> Module::FindUid(std::string_view name, std::optional<UID> sp) const {
    return std::nullopt;
}


std::optional<TableDesc> Module::FindTable(UID table) const {
    return std::nullopt;
}


std::optional<Type> Module::FindType(UID uid) const {
    return std::nullopt;
}

} // namespace sedmgr