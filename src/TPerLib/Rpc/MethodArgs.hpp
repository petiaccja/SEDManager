#pragma once

#include "../Communication/TokenStream.hpp"
#include "Types.hpp"

#include <concepts>
#include <optional>
#include <string>
#include <unordered_map>


// Integers
template <std::integral T>
TokenStream SerializeArg(T arg);
template <std::integral T>
void ParseArg(const TokenStream& stream, T& arg);

// String (bytes)
TokenStream SerializeArg(const std::string& arg);
TokenStream SerializeArg(std::string_view arg);
void ParseArg(const TokenStream& stream, std::string& arg);

// Uid (bytes)
TokenStream SerializeArg(const Uid& arg);
void ParseArg(const TokenStream& stream, Uid& arg);

// Range of std::byte (bytes)
template <class Range>
    requires std::same_as<std::byte, std::ranges::range_value_t<Range>>
TokenStream SerializeArg(const Range& arg);
void ParseArg(const TokenStream& stream, std::vector<std::byte>& arg);

// Optional (optional parameters)
template <class T>
TokenStream SerializeArg(const std::optional<T>& arg);
template <class T>
void ParseArg(const TokenStream& stream, std::optional<T>& arg);

// Unordered map (list of named values)
template <class Key, class Value>
TokenStream SerializeArg(const std::unordered_map<Key, Value>& arg);
template <class Key, class Value>
void ParseArg(const TokenStream& stream, std::unordered_map<Key, Value>& arg);



template <std::integral T>
TokenStream SerializeArg(T arg) {
    return arg;
}


template <std::integral T>
void ParseArg(const TokenStream& stream, T& arg) {
    if (!stream.IsInteger()) {
        throw std::invalid_argument("expected an integer");
    }
    arg = stream.Get<T>();
}


template <class Range>
    requires std::same_as<std::byte, std::ranges::range_value_t<Range>>
TokenStream SerializeArg(const Range& arg) {
    return { TokenStream::bytes, arg };
}


template <class T>
TokenStream SerializeArg(const std::optional<T>& arg) {
    if (arg.has_value()) {
        return SerializeArg(arg.value());
    }
    return {};
}


template <class T>
void ParseArg(const TokenStream& stream, std::optional<T>& arg) {
    if (stream.HasValue()) {
        T value;
        ParseArg(stream, value);
        arg = std::move(value);
    }
}


template <class Key, class Value>
TokenStream SerializeArg(const std::unordered_map<Key, Value>& arg) {
    std::vector<TokenStream> nameds;
    for (const auto& [key, value] : arg) {
        nameds.emplace_back(Named(SerializeArg(key), SerializeArg(value)));
    }
    return TokenStream(std::move(nameds));
}


template <class Key, class Value>
void ParseArg(const TokenStream& stream, std::unordered_map<Key, Value>& arg) {
    if (!stream.IsList()) {
        throw std::invalid_argument("expected a list of named values for unordered_map");
    }
    const auto& nameds = stream.Get<std::span<const TokenStream>>();
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
