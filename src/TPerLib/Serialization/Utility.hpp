#pragma once

#include "FlatBinaryArchive.hpp"
#include "RpcArchive.hpp"

#include <cstdint>
#include <span>
#include <sstream>
#include <vector>


template <class Object>
std::vector<uint8_t> ToBytes(const Object& object) {
    std::stringstream buffer;
    FlatBinaryOutputArchive ar(buffer);
    ar(object);
    const auto chars = buffer.view();
    const auto bytes = reinterpret_cast<const uint8_t*>(chars.data());
    return { bytes, bytes + chars.size() };
}


template <class Object>
void FromBytes(std::span<const uint8_t> bytes, Object& object) {
    std::stringstream buffer;
    const auto chars = reinterpret_cast<const char*>(bytes.data());
    const auto view = std::string_view{ chars, chars + bytes.size() };
    buffer.str(std::string{ view });
    FlatBinaryInputArchive ar(buffer);
    ar(object);
}


template <class Object>
std::vector<uint8_t> ToTokens(const Object& object) {
    std::stringstream buffer;
    RpcOutputArchive ar(buffer);
    ar(object);
    const auto chars = buffer.view();
    const auto bytes = reinterpret_cast<const uint8_t*>(chars.data());
    return { bytes, bytes + chars.size() };
}


template <class Object>
void FromTokens(std::span<const uint8_t> bytes, Object& object) {
    std::stringstream buffer;
    const auto chars = reinterpret_cast<const char*>(bytes.data());
    const auto view = std::string_view{ chars, chars + bytes.size() };
    buffer.str(std::string{ view });
    RpcInputArchive ar(buffer);
    ar(object);
}