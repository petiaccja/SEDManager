#pragma once

#include "FlatBinaryArchive.hpp"
#include "TokenArchive.hpp"

#include <cstdint>
#include <span>
#include <sstream>
#include <vector>


template <class Object>
std::vector<uint8_t> ToBytes(const Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::out);
    FlatBinaryOutputArchive ar(buffer);
    ar(object);
    const auto chars = buffer.view();
    const auto bytes = reinterpret_cast<const uint8_t*>(chars.data());
    return { bytes, bytes + chars.size() };
}


template <class Object>
void FromBytes(std::span<const uint8_t> bytes, Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::in);
    const auto chars = reinterpret_cast<const char*>(bytes.data());
    const auto view = std::string_view{ chars, chars + bytes.size() };
    buffer.str(std::string{ view });
    FlatBinaryInputArchive ar(buffer);
    ar(object);
}


inline std::span<const uint8_t> BytesView(std::string_view str) {
    const auto bytes = reinterpret_cast<const uint8_t*>(str.data());
    return { bytes, bytes + str.size() };
}


inline std::string_view StringView(std::span<const uint8_t> bytes) {
    const auto str = reinterpret_cast<const char*>(bytes.data());
    return { str, str + bytes.size() };
}


template <class Object>
std::vector<uint8_t> ToTokens(const Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::out);
    TokenOutputArchive ar(buffer);
    ar(object);
    const auto chars = buffer.view();
    const auto bytes = reinterpret_cast<const uint8_t*>(chars.data());
    return { bytes, bytes + chars.size() };
}


template <class Object>
void FromTokens(std::span<const uint8_t> bytes, Object& object) {
    std::stringstream buffer(std::ios::binary | std::ios::in);
    const auto chars = reinterpret_cast<const char*>(bytes.data());
    const auto view = std::string_view{ chars, chars + bytes.size() };
    buffer.str(std::string{ view });
    TokenInputArchive ar(buffer);
    ar(object);
}