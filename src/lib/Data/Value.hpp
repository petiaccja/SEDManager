#pragma once

#include <Error/Exception.hpp>

#include <algorithm>
#include <any>
#include <concepts>
#include <cstdint>
#include <format>
#include <optional>
#include <span>
#include <string>
#include <typeinfo>
#include <vector>


enum class eCommand : uint8_t {
    CALL = 0xF8,
    END_OF_DATA = 0xF9,
    END_OF_SESSION = 0xFA,
    START_TRANSACTION = 0xFB,
    END_TRANSACTION = 0xFC,
    EMPTY = 0xFF,
};


struct Named;


class Value {
    struct AsBytesType {};

public:
    static constexpr auto bytes = AsBytesType{};
    using ListType = std::vector<Value>;
    using BytesType = std::vector<std::byte>;
    using IntTypes = std::tuple<bool, char, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>;

public:
    Value() = default;

    Value(std::integral auto value);

    Value(eCommand command);

    Value(Named value);

    Value(std::initializer_list<Value> values);

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, Value>
    Value(R&& values);

    template <std::ranges::range R>
        requires std::same_as<std::ranges::range_value_t<R>, std::byte>
    Value(R&& bytes);


    template <std::integral T>
    T GetInt() const;

    eCommand GetCommand() const;

    const Named& GetNamed() const;

    std::span<const Value> GetList() const;

    std::span<const std::byte> GetBytes() const;


    std::vector<Value>& GetList();

    Named& GetNamed();

    std::vector<std::byte>& GetBytes();


    template <std::integral T>
    T Get() const;

    template <std::same_as<eCommand> T>
    eCommand Get() const;

    template <std::same_as<Named> T>
    const Named& Get() const;

    template <std::same_as<std::span<const Value>> T>
    std::span<const Value> Get() const;

    template <std::same_as<std::span<const std::byte>> T>
    std::span<const std::byte> Get() const;


    template <std::same_as<Named> T>
    Named& Get();

    template <std::same_as<std::vector<Value>> T>
    std::vector<Value>& Get();

    template <std::same_as<std::span<Value>> T>
    std::span<Value> Get();

    template <std::same_as<std::vector<std::byte>> T>
    std::vector<std::byte>& Get();

    template <std::same_as<std::span<std::byte>> T>
    std::span<std::byte> Get();


    bool IsInteger() const;
    bool IsBytes() const;
    bool IsList() const;
    bool IsNamed() const;
    bool IsCommand() const;
    bool HasValue() const { return m_value.has_value(); }
    const std::type_info& Type() const;
    std::string GetTypeStr() const;

    bool operator==(const Value& rhs) const;
    bool operator!=(const Value& rhs) const { return !operator==(rhs); }

private:
    std::any m_value;
};


struct Named {
    Value name;
    Value value;
};


namespace impl {

template <class Func, class P, class... Ps>
decltype(auto) ForEachTypeHelper(Func func, const P* ptr, const Ps*... ptrs) {
    std::optional value = func(ptr);
    if (value) {
        return value;
    }
    if constexpr (sizeof...(ptrs) == 0) {
        return decltype(value){ std::nullopt };
    }
    else {
        return ForEachTypeHelper(func, ptrs...);
    }
}

template <class TupleType, class Func, size_t... Indices>
decltype(auto) ForEachTypeHelper(Func func, std::index_sequence<Indices...>) {
    std::tuple ptrs{ static_cast<std::tuple_element_t<Indices, TupleType>*>(nullptr)... };
    const auto dispatch = [&func](const auto*... ptrs) {
        return ForEachTypeHelper(func, ptrs...);
    };
    return std::apply(dispatch, ptrs);
}


template <class TupleType, class Func>
decltype(auto) ForEachType(Func func) {
    return ForEachTypeHelper<TupleType>(std::move(func), std::make_index_sequence<std::tuple_size_v<TupleType>>());
}

} // namespace impl


Value::Value(std::integral auto value)
    : m_value(value) {}


template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, Value>
Value::Value(R&& values)
    : m_value(ListType(std::ranges::begin(values), std::ranges::end(values))) {}


template <std::ranges::range R>
    requires std::same_as<std::ranges::range_value_t<R>, std::byte>
Value::Value(R&& values) {
    m_value = BytesType(std::ranges::begin(values), std::ranges::end(values));
}

template <std::integral T>
T Value::GetInt() const {
    const auto v = impl::ForEachType<IntTypes>([this](auto* ptr) -> std::optional<T> {
        using Q = std::decay_t<decltype(*ptr)>;
        if (m_value.type() == typeid(Q)) {
            return static_cast<T>(std::any_cast<const Q&>(m_value));
        }
        return std::nullopt;
    });
    if (!v) {
        const auto targetType = std::format("{}int{}", std::is_signed_v<T> ? "" : "u", sizeof(T) * 8);
        const auto actualType = GetTypeStr();
        throw TypeConversionError(actualType, targetType);
    }
    return *v;
}


template <std::integral T>
T Value::Get() const {
    return GetInt<T>();
}

template <std::same_as<std::vector<std::byte>> T>
std::vector<std::byte>& Value::Get() {
    return GetBytes();
}

template <std::same_as<std::span<std::byte>> T>
std::span<std::byte> Value::Get() {
    return GetBytes();
}

template <std::same_as<std::span<const std::byte>> T>
std::span<const std::byte> Value::Get() const {
    return GetBytes();
}

template <std::same_as<std::span<const Value>> T>
std::span<const Value> Value::Get() const {
    return GetList();
}

template <std::same_as<Named> T>
const Named& Value::Get() const {
    return GetNamed();
}

template <std::same_as<eCommand> T>
eCommand Value::Get() const {
    return GetCommand();
}

template <std::same_as<std::vector<Value>> T>
std::vector<Value>& Value::Get() {
    return GetList();
}


template <std::same_as<std::span<Value>> T>
std::span<Value> Value::Get() {
    return GetList();
}

template <std::same_as<Named> T>
Named& Value::Get() {
    return GetNamed();
}
