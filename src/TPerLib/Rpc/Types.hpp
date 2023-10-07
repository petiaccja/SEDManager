#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>


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
    operator Integral() const noexcept { return static_cast<Integral>(value); }

    template <class Enum>
        requires std::is_enum_v<Enum> && (sizeof(Enum) <= sizeof(uint64_t))
    operator Enum() const noexcept { return static_cast<Enum>(value); }

    std::strong_ordering operator<=>(const Uid&) const noexcept = default;

    uint64_t value = 0;
};
