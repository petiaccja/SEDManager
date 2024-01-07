#pragma once

#include <cstdint>
#include <string_view>


namespace sedmgr {

struct UID {
    UID() = default;

    template <std::integral Integral>
        requires(sizeof(Integral) <= sizeof(uint64_t))
    constexpr UID(Integral value) noexcept : value(value) {}

    template <class Enum>
        requires std::is_enum_v<Enum> && (sizeof(Enum) <= sizeof(uint64_t))
    constexpr UID(Enum value) noexcept : value(static_cast<uint64_t>(value)) {}

    template <std::integral Integral>
        requires(sizeof(Integral) >= sizeof(uint64_t))
    constexpr operator Integral() const noexcept { return static_cast<Integral>(value); }

    template <class Enum>
        requires std::is_enum_v<Enum> && (sizeof(Enum) <= sizeof(uint64_t))
    constexpr operator Enum() const noexcept { return static_cast<Enum>(value); }

    std::strong_ordering operator<=>(const UID&) const noexcept = default;

    uint64_t value = 0;
};


std::string to_string(UID uid);
UID stouid(std::string_view str);


template <class Archive>
void save(Archive& ar, const UID& object) {
    ar(uint64_t(object));
}


template <class Archive>
void load(Archive& ar, UID& object) {
    uint64_t value = 0;
    ar(value);
    object = UID(value);
}

} // namespace sedmgr


namespace std {

template <>
struct hash<sedmgr::UID> {
    auto operator()(const sedmgr::UID& uid) const {
        return std::hash<uint64_t>()(uid);
    }
};

} // namespace std
