#pragma once

#include "FlatBinaryArchive.hpp"

#include <cstdint>
#include <span>
#include <sstream>
#include <vector>


namespace sedmgr {

constexpr std::byte operator""_b(unsigned long long value) {
    return std::byte(value);
}


template <class T>
struct Serialized {
    std::span<const std::byte> bytes;
};


template <class Object>
    requires requires(const Object& obj, FlatBinaryOutputArchive& ar) {
                 {
                     ar(obj)
                 };
             }
std::vector<std::byte> Serialize(const Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::out);
    FlatBinaryOutputArchive ar(buffer);
    ar(object);
    const auto chars = buffer.view();
    const auto bytes = reinterpret_cast<const std::byte*>(chars.data());
    return { bytes, bytes + chars.size() };
}


template <class Object>
    requires requires(Object& obj, FlatBinaryInputArchive& ar) {
                 {
                     ar(obj)
                 };
             }
Object DeSerialize(const Serialized<Object>& bytes) {
    std::stringstream buffer(std::ios::binary | std::ios::in);
    const auto chars = reinterpret_cast<const char*>(bytes.bytes.data());
    const auto view = std::string_view{ chars, chars + bytes.bytes.size() };
    buffer.str(std::string{ view });
    FlatBinaryInputArchive ar(buffer);
    Object object;
    ar(object);
    return object;
}


inline std::string_view ToStringView(std::span<const uint8_t> bytes) {
    const auto str = reinterpret_cast<const char*>(bytes.data());
    return { str, str + bytes.size() };
}


inline std::string_view ToStringView(std::span<const std::byte> bytes) {
    const auto str = reinterpret_cast<const char*>(bytes.data());
    return { str, str + bytes.size() };
}

} // namespace sedmgr