#pragma once

#include <compare>
#include <concepts>
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <variant>


struct Uid {
    Uid() = default;

    template <std::integral Integral>
        requires(sizeof(Integral) <= sizeof(uint64_t))
    constexpr Uid(Integral value) noexcept : value(value) {}

    template <class Enum>
        requires std::is_enum_v<Enum> && (sizeof(Enum) <= sizeof(uint64_t))
    constexpr Uid(Enum value) noexcept : value(static_cast<uint64_t>(value)) {}

    template <std::integral Integral>
        requires(sizeof(Integral) >= sizeof(uint64_t))
    constexpr operator Integral() const noexcept { return static_cast<Integral>(value); }

    template <class Enum>
        requires std::is_enum_v<Enum> && (sizeof(Enum) <= sizeof(uint64_t))
    constexpr operator Enum() const noexcept { return static_cast<Enum>(value); }

    std::strong_ordering operator<=>(const Uid&) const noexcept = default;

    uint64_t value = 0;
};


struct CellBlock {
    std::optional<std::variant<Uid, uint32_t>> startRow;
    std::optional<uint32_t> endRow;
    std::optional<uint32_t> startColumn;
    std::optional<uint32_t> endColumn;

    std::strong_ordering operator<=>(const CellBlock&) const noexcept = default;
};


std::string to_string(Uid uid);
Uid stouid(std::string_view str);


namespace std {

template <>
struct hash<Uid> {
    auto operator()(const Uid& uid) const {
        return std::hash<uint64_t>()(uid);
    }
};

} // namespace std