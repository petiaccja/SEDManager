#pragma once

#include "../Communication/Value.hpp"
#include "Types.hpp"

#include <concepts>
#include <optional>
#include <string>
#include <unordered_map>


// Value
Value ToValue(const Value& arg);
void FromValue(const Value& value, Value& arg);

// Integers
template <std::integral T>
Value ToValue(T arg);
template <std::integral T>
void FromValue(const Value& value, T& arg);

// String (bytes)
Value ToValue(const std::string& arg);
Value ToValue(std::string_view arg);
void FromValue(const Value& value, std::string& arg);

// Uid (bytes)
Value ToValue(const Uid& arg);
void FromValue(const Value& value, Uid& arg);

// CellBlock
Value ToValue(const CellBlock& arg);
void FromValue(const Value& value, CellBlock& arg);

// Range of std::byte (bytes)
template <class Range>
    requires std::same_as<std::byte, std::ranges::range_value_t<Range>>
Value ToValue(const Range& arg);
void FromValue(const Value& value, std::vector<std::byte>& arg);

// Optional (optional parameters)
template <class T>
Value ToValue(const std::optional<T>& arg);
template <class T>
void FromValue(const Value& value, std::optional<T>& arg);

// Unordered map (list of named values)
template <class Key, class ValueT>
Value ToValue(const std::unordered_map<Key, ValueT>& arg);
template <class Key, class ValueT>
void FromValue(const Value& value, std::unordered_map<Key, ValueT>& arg);



template <std::integral T>
Value ToValue(T arg) {
    return arg;
}


template <std::integral T>
void FromValue(const Value& stream, T& arg) {
    if (!stream.IsInteger()) {
        throw std::invalid_argument("expected an integer");
    }
    arg = stream.Get<T>();
}


template <class Range>
    requires std::same_as<std::byte, std::ranges::range_value_t<Range>>
Value ToValue(const Range& arg) {
    return { Value::bytes, arg };
}


template <class T>
Value ToValue(const std::optional<T>& arg) {
    if (arg.has_value()) {
        return ToValue(arg.value());
    }
    return {};
}


template <class T>
void FromValue(const Value& stream, std::optional<T>& arg) {
    if (stream.HasValue()) {
        T value;
        FromValue(stream, value);
        arg = std::move(value);
    }
}


template <class Key, class ValueT>
Value ToValue(const std::unordered_map<Key, ValueT>& arg) {
    std::vector<Value> nameds;
    for (const auto& [key, value] : arg) {
        nameds.emplace_back(Named(ToValue(key), ToValue(value)));
    }
    return Value(std::move(nameds));
}


template <class Key, class ValueT>
void FromValue(const Value& stream, std::unordered_map<Key, ValueT>& arg) {
    if (!stream.IsList()) {
        throw std::invalid_argument("expected a list of named values for unordered_map");
    }
    const auto& nameds = stream.Get<std::span<const Value>>();
    arg = {};
    for (auto& named : nameds) {
        if (!named.IsNamed()) {
            throw std::invalid_argument("expected a named value within unordered_map");
        }
        Key key;
        ValueT value;
        FromValue(named.AsNamed().name, key);
        FromValue(named.AsNamed().value, value);
        arg.insert_or_assign(std::move(key), std::move(value));
    }
}
