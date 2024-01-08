#include "UID.hpp"

#include <format>
#include <string>


namespace sedmgr {

std::string UID::ToString() const {
    std::string result;
    result.reserve(19);
    constexpr std::string_view fmt = "{:04X}";
    result += std::format(fmt, uint16_t(value >> 48));
    result += '\'';
    result += std::format(fmt, uint16_t(value >> 32));
    result += '\'';
    result += std::format(fmt, uint16_t(value >> 16));
    result += '\'';
    result += std::format(fmt, uint16_t(value));
    return result;
}


UID UID::Parse(std::string_view str) {
    std::string withoutSeparators;
    for (auto c : str) {
        if (c != '\'') {
            withoutSeparators.push_back(c);
        }
    }
    size_t idx;
    const uint64_t value = std::stoull(withoutSeparators, &idx, 16);
    if (idx != withoutSeparators.size()) {
        throw std::invalid_argument("invalid character in UID string");
    }
    return UID(value);
}

} // namespace sedmgr