#include "UID.hpp"

#include <format>
#include <string>


namespace sedmgr {

std::string to_string(UID uid) {
    std::string result;
    result.reserve(19);
    constexpr std::string_view fmt = "{:04X}";
    result += std::format(fmt, uint16_t(uint64_t(uid) >> 48));
    result += '\'';
    result += std::format(fmt, uint16_t(uint64_t(uid) >> 32));
    result += '\'';
    result += std::format(fmt, uint16_t(uint64_t(uid) >> 16));
    result += '\'';
    result += std::format(fmt, uint16_t(uint64_t(uid)));
    return result;
}


UID stouid(std::string_view str) {
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
    return value;
}

} // namespace sedmgr