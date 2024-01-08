#pragma once

#include <cstdint>
#include <stdexcept>
#include <string_view>


namespace sedmgr {

struct UID {
    UID() : value(0) {}

    template <std::integral Integral>
        requires(sizeof(Integral) <= sizeof(uint64_t))
    explicit constexpr UID(Integral value) noexcept : value(value) {}

    template <class Enum>
        requires std::is_enum_v<Enum> && (sizeof(Enum) <= sizeof(uint64_t))
    explicit constexpr UID(Enum value) noexcept : value(static_cast<uint64_t>(value)) {}

    template <std::integral Integral>
        requires(sizeof(Integral) >= sizeof(uint64_t))
    explicit constexpr operator Integral() const noexcept { return static_cast<Integral>(value); }

    template <class Enum>
        requires std::is_enum_v<Enum> && (sizeof(Enum) <= sizeof(uint64_t))
    explicit constexpr operator Enum() const noexcept { return static_cast<Enum>(value); }

    constexpr UID ToDescriptor() const {
        if (!IsTable()) {
            throw std::logic_error("expected a table");
        }
        return UID((value >> 32) | (1ull << 32));
    }

    constexpr UID ToTable() const {
        if (!IsDescriptor()) {
            throw std::logic_error("expected a table descriptor");
        }
        return UID(value << 32);
    }

    constexpr UID ContainingTable() const {
        if (!IsObject()) {
            throw std::logic_error("expected an object");
        }
        return UID(value & 0xFFFF'FFFF'0000'0000ull);
    }

    constexpr bool IsTable() const {
        return (value & 0x0000'0000'FFFF'FFFFull) == 0;
    }

    constexpr bool IsDescriptor() const {
        return (value & 0xFFFF'FFFF'0000'0000ull) == 0x0000'0001'0000'0000ull
               && (value & 0x0000'0000'FFFF'FFFFull) != 0;
    }

    constexpr bool IsObject() const {
        return !IsTable() && !IsDescriptor();
    }

    std::string ToString() const;
    static UID Parse(std::string_view str);

    std::strong_ordering operator<=>(const UID&) const noexcept = default;

    uint64_t value = 0;
};


constexpr UID operator""_uid(unsigned long long value) {
    return UID(value);
}


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
        return std::hash<uint64_t>()(uid.value);
    }
};

} // namespace std
