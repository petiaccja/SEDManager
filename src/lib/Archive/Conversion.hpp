#pragma once

#include "FlatBinaryArchive.hpp"
#include "TokenBinaryArchive.hpp"

#include <cstdint>
#include <span>
#include <sstream>
#include <vector>


namespace sedmgr {

constexpr std::byte operator""_b(unsigned long long value) {
    return std::byte(value);
}


template <class Object>
std::vector<std::byte> ToBytes(const Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::out);
    FlatBinaryOutputArchive ar(buffer);
    ar(object);
    const auto chars = buffer.view();
    const auto bytes = reinterpret_cast<const std::byte*>(chars.data());
    return { bytes, bytes + chars.size() };
}


template <class Object>
void FromBytes(std::span<const std::byte> bytes, Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::in);
    const auto chars = reinterpret_cast<const char*>(bytes.data());
    const auto view = std::string_view{ chars, chars + bytes.size() };
    buffer.str(std::string{ view });
    FlatBinaryInputArchive ar(buffer);
    ar(object);
}


inline std::string_view ToStringView(std::span<const uint8_t> bytes) {
    const auto str = reinterpret_cast<const char*>(bytes.data());
    return { str, str + bytes.size() };
}


inline std::string_view ToStringView(std::span<const std::byte> bytes) {
    const auto str = reinterpret_cast<const char*>(bytes.data());
    return { str, str + bytes.size() };
}


template <class Object>
std::vector<std::byte> ToTokens(const Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::out);
    TokenBinaryOutputArchive ar(buffer);
    ar(object);
    const auto chars = buffer.view();
    const auto bytes = reinterpret_cast<const std::byte*>(chars.data());
    return { bytes, bytes + chars.size() };
}


template <class Object>
void FromTokens(std::span<const std::byte> bytes, Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::in);
    const auto chars = reinterpret_cast<const char*>(bytes.data());
    const auto view = std::string_view{ chars, chars + bytes.size() };
    buffer.str(std::string{ view });
    TokenBinaryInputArchive ar(buffer);
    ar(object);
}

} // namespace sedmgr