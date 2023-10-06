#pragma once

#include "Serialization/RpcStream.hpp"

#include <concepts>
#include <optional>
#include <string>
#include <unordered_map>


template <std::integral T>
RpcStream SerializeArg(T arg);
template <std::integral T>
void ParseArg(const RpcStream& stream, T& arg);

RpcStream SerializeArg(const std::string& arg);
void ParseArg(const RpcStream& stream, std::string& arg);

template <class T>
RpcStream SerializeArg(const std::optional<T>& arg);
template <class T>
void ParseArg(const RpcStream& stream, std::optional<T>& arg);

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
