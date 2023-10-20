#include "Module.hpp"



std::span<const std::shared_ptr<Module>> Module::Features() const {
    return {};
}


std::optional<std::string> Module::FindName(Uid uid) const {
    return std::nullopt;
}


std::optional<Uid> Module::FindUid(std::string_view name) const {
    return std::nullopt;
}


std::optional<TableDesc> Module::FindTable(Uid table) const {
    return std::nullopt;
}


std::optional<Type> Module::FindType(Uid uid) const {
    return std::nullopt;
}