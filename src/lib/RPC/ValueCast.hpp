#pragma once

#include "Value.hpp"

#include <Archive/Conversion.hpp>
#include <Specification/Types.hpp>

#include <concepts>
#include <optional>
#include <string>
#include <unordered_map>



template <class T>
    requires(!std::same_as<T, Value>)
T value_cast(const Value& v);


template <class T>
Value value_cast(const T& v);


namespace impl {

template <class T>
struct ValueCast;


template <>
struct ValueCast<Value> {
    static Value To(const Value& v) { return v; }
    static Value From(const Value& v) { return v; }
};


template <class T>
struct ValueCast<std::optional<T>> {
    static Value To(const std::optional<T>& v) { return v.has_value() ? value_cast(v.value()) : Value{}; }
    static std::optional<T> From(const Value& v) { return v.HasValue() ? std::optional<T>{ v.Get<T>() } : std::optional<T>{}; }
};


template <std::integral T>
struct ValueCast<T> {
    static Value To(const T& v) { return Value(v); }
    static T From(const Value& v) { return v.Get<T>(); }
};


template <>
struct ValueCast<std::string> {
    static Value To(const std::string& v) { return Value(std::as_bytes(std::span(v))); }
    static std::string From(const Value& v) { return std::string(ToStringView(v.Get<std::span<const std::byte>>())); }
};


template <>
struct ValueCast<std::string_view> {
    static Value To(const std::string_view& v) { return Value(std::as_bytes(std::span(v))); }
    static std::string_view From(const Value& v) {
        const auto bytes = v.AsBytes();
        const auto ptr = reinterpret_cast<const char*>(bytes.data());
        return { ptr, ptr + bytes.size() };
    };
};


template <>
struct ValueCast<Uid> {
    static Value To(const Uid& v) { return Value(ToBytes(uint64_t(v))); }
    static Uid From(const Value& v) {
        uint64_t uid;
        FromBytes(v.AsBytes(), uid);
        return Uid(uid);
    };
};


template <>
struct ValueCast<CellBlock> {
    static Value To(const CellBlock& v) {
        std::vector<Value> fields;
        if (v.startRow.has_value()) {
            auto valOfNamed = std::visit([](const auto& v) { return value_cast(v); }, *v.startRow);
            fields.emplace_back(Named{ 1u, std::move(valOfNamed) });
        }
        if (v.endRow.has_value()) {
            fields.emplace_back(Named{ 2u, v.endRow.value() });
        }
        if (v.startColumn) {
            fields.emplace_back(Named{ 3u, v.startColumn.value() });
        }
        if (v.endColumn) {
            fields.emplace_back(Named{ 4u, v.endColumn.value() });
        }
        return fields;
    }
    static CellBlock From(const Value& v) {
        if (!v.IsList()) {
            throw std::invalid_argument("expected a list");
        }
        const auto fields = v.Get<std::span<const Value>>();
        CellBlock parsed;
        for (const auto& field : fields) {
            const auto& named = field.Get<Named>();
            const auto id = named.name.Get<uint32_t>();
            switch (id) {
                case 0: break; // Table name
                case 1: parsed.startRow = named.value.IsInteger() ? value_cast<uint32_t>(named.value) : value_cast<Uid>(named.value); break;
                case 2: parsed.endRow = named.value.HasValue() ? std::optional(named.value.Get<uint32_t>()) : std::optional<uint32_t>{}; break;
                case 3: parsed.startColumn = named.value.Get<uint32_t>(); break;
                case 4: parsed.endColumn = named.value.Get<uint32_t>(); break;
                default: throw std::invalid_argument("invalid field for cell block");
            }
        }
        return std::move(parsed);
    }
};


template <std::ranges::range Range>
    requires std::same_as<std::ranges::range_value_t<Range>, std::byte>
struct ValueCast<Range> {
    static Value To(const Range& v) {
        std::vector b(std::begin(v), std::end(v));
        return Value(b);
    }
    static Range From(const Value& v) {
        if constexpr (requires(Range& r, std::ranges::range_value_t<Range>&& v) { r.push_back(v); }) {
            Range r;
            for (const auto byte : v.Get<std::span<const std::byte>>()) {
                r.push_back(std::bit_cast<std::ranges::range_value_t<Range>>(byte));
            }
            return r;
        }
        else {
            static_assert(sizeof(Range) == 0, "container needs to support push_back");
        }
    }
};


template <std::ranges::range Range>
    requires(!std::same_as<std::ranges::range_value_t<Range>, std::byte>)
struct ValueCast<Range> {
    static Value To(const Range& v) {
        std::vector<Value> values;
        for (auto& item : v) {
            values.push_back(value_cast(item));
        }
        return Value(std::move(values));
    }
    static Range From(const Value& v) {
        if constexpr (requires(Range& r, std::ranges::range_value_t<Range>&& v) { r.push_back(v); }) {
            Range r;
            for (auto& item : v.AsList()) {
                r.push_back(value_cast<std::ranges::range_value_t<Range>>(item));
            }
            return r;
        }
        else {
            static_assert(sizeof(Range) == 0, "container needs to support push_back");
        }
    }
};


template <class K, class V>
struct ValueCast<std::unordered_map<K, V>> {
    static Value To(const std::unordered_map<K, V>& v) {
        std::vector<Value> nameds;
        for (const auto& [key, value] : v) {
            nameds.emplace_back(Named(value_cast(key), value_cast(value)));
        }
        return Value(std::move(nameds));
    }
    static std::unordered_map<K, V> From(const Value& v) {
        if (!v.IsList()) {
            throw std::invalid_argument("expected a list of named values for unordered_map");
        }
        const auto& nameds = v.Get<std::span<const Value>>();
        std::unordered_map<K, V> mapped = {};
        for (auto& named : nameds) {
            if (!named.IsNamed()) {
                throw std::invalid_argument("expected a named value within unordered_map");
            }
            mapped.insert_or_assign(value_cast<K>(named.AsNamed().name), value_cast<V>(named.AsNamed().value));
        }
        return mapped;
    }
};



} // namespace impl


template <class T>
    requires(!std::same_as<T, Value>)
T value_cast(const Value& v) {
    return impl::ValueCast<T>::From(v);
}


template <class T>
Value value_cast(const T& v) {
    return impl::ValueCast<T>::To(v);
}