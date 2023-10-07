#pragma once

#include "Serialization/RpcStream.hpp"

#include <concepts>
#include <optional>
#include <string>
#include <unordered_map>


using UID = std::array<std::byte, 8>;

constexpr std::byte operator""_b(unsigned long long value) {
    return std::byte(value);
}


// Integers
template <std::integral T>
RpcStream SerializeArg(T arg);
template <std::integral T>
void ParseArg(const RpcStream& stream, T& arg);

// String (bytes)
RpcStream SerializeArg(const std::string& arg);
RpcStream SerializeArg(std::string_view arg);
void ParseArg(const RpcStream& stream, std::string& arg);

// Range of std::byte (bytes)
template <class Range>
    requires std::same_as<std::byte, std::ranges::range_value_t<Range>>
RpcStream SerializeArg(const Range& arg);
void ParseArg(const RpcStream& stream, std::vector<std::byte>& arg);

// Optional (optional parameters)
template <class T>
RpcStream SerializeArg(const std::optional<T>& arg);
template <class T>
void ParseArg(const RpcStream& stream, std::optional<T>& arg);

// Unordered map (list of named values)
template <class Key, class Value>
RpcStream SerializeArg(const std::unordered_map<Key, Value>& arg);
template <class Key, class Value>
void ParseArg(const RpcStream& stream, std::unordered_map<Key, Value>& arg);



template <std::integral T>
RpcStream SerializeArg(T arg) {
    return arg;
}


template <std::integral T>
void ParseArg(const RpcStream& stream, T& arg) {
    if (!stream.IsInteger()) {
        throw std::invalid_argument("expected an integer");
    }
    arg = stream.Get<T>();
}


template <class Range>
    requires std::same_as<std::byte, std::ranges::range_value_t<Range>>
RpcStream SerializeArg(const Range& arg) {
    return { RpcStream::bytes, arg };
}


template <class T>
RpcStream SerializeArg(const std::optional<T>& arg) {
    if (arg.has_value()) {
        return SerializeArg(arg.value());
    }
    return {};
}


template <class T>
void ParseArg(const RpcStream& stream, std::optional<T>& arg) {
    if (stream.HasValue()) {
        T value;
        ParseArg(stream, value);
        arg = std::move(value);
    }
}


template <class Key, class Value>
RpcStream SerializeArg(const std::unordered_map<Key, Value>& arg) {
    std::vector<RpcStream> nameds;
    for (const auto& [key, value] : arg) {
        nameds.emplace_back(Named(SerializeArg(key), SerializeArg(value)));
    }
    return RpcStream(std::move(nameds));
}


template <class Key, class Value>
void ParseArg(const RpcStream& stream, std::unordered_map<Key, Value>& arg) {
    if (!stream.IsList()) {
        throw std::invalid_argument("expected a list of named values for unordered_map");
    }
    const auto& nameds = stream.Get<std::span<const RpcStream>>();
    arg = {};
    for (auto& named : nameds) {
        if (!named.IsNamed()) {
            throw std::invalid_argument("expected a named value within unordered_map");
        }
        Key key;
        Value value;
        ParseArg(named.AsNamed().name, key);
        ParseArg(named.AsNamed().value, value);
        arg.insert_or_assign(std::move(key), std::move(value));
    }
}
