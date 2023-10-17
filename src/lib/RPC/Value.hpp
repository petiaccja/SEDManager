#pragma once

#include <Archive/FlatBinaryArchive.hpp>
#include <Archive/Token.hpp>

#include <cereal/cereal.hpp>

#include <algorithm>
#include <any>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <optional>
#include <span>
#include <stack>
#include <string>
#include <string_view>
#include <typeinfo>
#include <vector>


enum class eCommand : uint8_t {
    CALL = static_cast<std::underlying_type_t<eTag>>(eTag::CALL),
    END_OF_DATA = static_cast<std::underlying_type_t<eTag>>(eTag::END_OF_DATA),
    END_OF_SESSION = static_cast<std::underlying_type_t<eTag>>(eTag::END_OF_SESSION),
    START_TRANSACTION = static_cast<std::underlying_type_t<eTag>>(eTag::START_TRANSACTION),
    END_TRANSACTION = static_cast<std::underlying_type_t<eTag>>(eTag::END_TRANSACTION),
    EMPTY = static_cast<std::underlying_type_t<eTag>>(eTag::EMPTY),
};


struct Named;

template <class T>
struct is_std_span : std::bool_constant<false> {};

template <class T>
struct is_std_span<std::span<T>> : std::bool_constant<true> {};

template <class T>
struct is_const_std_span : std::bool_constant<false> {};

template <class T>
struct is_const_std_span<std::span<const T>> : std::bool_constant<true> {};


struct ValueConversionError : std::logic_error {
    ValueConversionError(std::string_view expected, std::string_view actual)
        : std::logic_error(std::format("expected a value of '{}' but got a value of '{}'", expected, actual)) {}
};


class Value {
    struct AsBytesType {};

public:
    static constexpr auto bytes = AsBytesType{};
    using ListType = std::vector<Value>;
    using BytesType = std::vector<std::byte>;
    using IntTypes = std::tuple<bool, char, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>;

public:
    Value() = default;

    //----------------------------------
    // Construction
    //----------------------------------

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

    //----------------------------------
    // Get
    //----------------------------------

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

    //----------------------------------
    // Get specific
    //----------------------------------

    // Integers.
    template <std::integral T>
    T AsInt() const;

    eCommand AsCommand() const;

    const Named& AsNamed() const;
    Named& AsNamed();

    std::span<const Value> AsList() const;
    std::vector<Value>& AsList();

    std::span<const std::byte> AsBytes() const;
    std::vector<std::byte>& AsBytes();

    //----------------------------------
    // Query type
    //----------------------------------

    bool IsInteger() const;
    bool IsBytes() const;
    bool IsList() const;
    bool IsNamed() const;
    bool IsCommand() const;
    bool HasValue() const { return m_value.has_value(); }
    const std::type_info& Type() const;
    std::string GetTypeStr() const;

private:

private:
    std::any m_value;
};


struct Named {
    Value name;
    Value value;
};

//------------------------------------------------------------------------------
// Utility
//------------------------------------------------------------------------------

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


//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

// Integers.
Value::Value(std::integral auto value)
    : m_value(value) {}


// Lists.
template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, Value>
Value::Value(R&& values)
    : m_value(ListType(std::ranges::begin(values), std::ranges::end(values))) {}


// Bytes.
template <std::ranges::range R>
    requires std::same_as<std::ranges::range_value_t<R>, std::byte>
Value::Value(R&& values) {
    m_value = BytesType(std::ranges::begin(values), std::ranges::end(values));
}

//------------------------------------------------------------------------------
// Lookup
//------------------------------------------------------------------------------


template <std::integral T>
T Value::AsInt() const {
    const auto v = ForEachType<IntTypes>([this](auto* ptr) -> std::optional<T> {
        using Q = std::decay_t<decltype(*ptr)>;
        if (m_value.type() == typeid(Q)) {
            return static_cast<T>(std::any_cast<const Q&>(m_value));
        }
        return std::nullopt;
    });
    if (!v) {
        const auto expected = std::format("{}int{}", std::is_signed_v<T> ? "" : "u", sizeof(T) * 8);
        const auto actual = GetTypeStr();
        throw ValueConversionError(expected, actual);
    }
    return *v;
}


template <std::integral T>
T Value::Get() const {
    return AsInt<T>();
}

template <std::same_as<std::vector<std::byte>> T>
std::vector<std::byte>& Value::Get() {
    return AsBytes();
}

template <std::same_as<std::span<std::byte>> T>
std::span<std::byte> Value::Get() {
    return AsBytes();
}

template <std::same_as<std::span<const std::byte>> T>
std::span<const std::byte> Value::Get() const {
    return AsBytes();
}

template <std::same_as<std::span<const Value>> T>
std::span<const Value> Value::Get() const {
    return AsList();
}

template <std::same_as<Named> T>
const Named& Value::Get() const {
    return AsNamed();
}

template <std::same_as<eCommand> T>
eCommand Value::Get() const {
    return AsCommand();
}

template <std::same_as<std::vector<Value>> T>
std::vector<Value>& Value::Get() {
    return AsList();
}


template <std::same_as<std::span<Value>> T>
std::span<Value> Value::Get() {
    return AsList();
}

template <std::same_as<Named> T>
Named& Value::Get() {
    return AsNamed();
}


//------------------------------------------------------------------------------
// Serialization
//------------------------------------------------------------------------------

template <class Archive>
void SaveInteger(Archive& ar, const Value& stream) {
    std::optional<bool> r = ForEachType<Value::IntTypes>([&]<class T>(T* ptr) -> std::optional<bool> {
        if (stream.Type() == typeid(T)) {
            const auto bytes = ToFlatBinary(stream.Get<T>());
            const Token token{
                .tag = eTag::SHORT_ATOM,
                .isByte = false,
                .isSigned = std::is_signed_v<T>,
                .data = { begin(bytes), end(bytes) },
            };
            ar(token);
            return { true };
        }
        return std::nullopt;
    });
    if (!r) {
        throw std::invalid_argument("stream is not an int");
    }
}


template <class Archive>
void SaveList(Archive& ar, const Value& stream) {
    ar(Token{ .tag = eTag::START_LIST });
    size_t idx = 0;
    for (const auto& item : stream.AsList()) {
        SaveDispatch(ar, item);
    }
    ar(Token{ .tag = eTag::END_LIST });
}


template <class Archive>
void SaveBytes(Archive& ar, const Value& stream) {
    const auto bytes = stream.AsBytes();
    Token token{
        .tag = GetTagForData(bytes.size_bytes()),
        .isByte = true,
        .isSigned = false,
        .data = { bytes.begin(), bytes.end() },
    };
    ar(token);
}


template <class Archive>
void SaveNamed(Archive& ar, const Value& stream) {
    ar(Token{ .tag = eTag::START_NAME });
    SaveDispatch(ar, stream.AsNamed().name);
    SaveDispatch(ar, stream.AsNamed().value);
    ar(Token{ .tag = eTag::END_NAME });
}


template <class Archive>
void SaveCommand(Archive& ar, const Value& stream) {
    ar(Token{ .tag = static_cast<eTag>(stream.AsCommand()) });
}


template <class Archive>
void SaveDispatch(Archive& ar, const Value& stream) {
    if (stream.IsInteger()) {
        SaveInteger(ar, stream);
    }
    if (stream.IsBytes()) {
        SaveBytes(ar, stream);
    }
    if (stream.IsList()) {
        SaveList(ar, stream);
    }
    if (stream.IsNamed()) {
        SaveNamed(ar, stream);
    }
    if (stream.IsCommand()) {
        SaveCommand(ar, stream);
    }
}


template <class Archive>
void save_strip_list(Archive& ar, const Value& stream) {
    if (stream.IsList()) {
        for (const auto& item : stream.AsList()) {
            SaveDispatch(ar, item);
        }
    }
    else {
        SaveDispatch(ar, stream);
    }
}


template <class Archive>
void save(Archive& ar, const Value& stream) {
    SaveDispatch(ar, stream);
}


Value ConvertToData(const Token& token);

void InsertItem(Value& target, Value item);


template <class Archive>
void load(Archive& ar, Value& stream) {
    std::stack<Value> stack;
    stack.push(Value(std::vector<Value>{}));
    do {
        Token token;
        try {
            ar(token);
        }
        catch (std::exception& ex) {
            break;
        }

        if (token.tag == eTag::EMPTY) {
            continue;
        }
        else if (token.tag == eTag::START_LIST) {
            stack.push(Value(std::vector<Value>{}));
        }
        else if (token.tag == eTag::START_NAME) {
            stack.push(Value(Named{}));
        }
        else if (token.tag == eTag::END_LIST) {
            auto item = std::move(stack.top());
            stack.pop();
            InsertItem(stack.top(), std::move(item));
        }
        else if (token.tag == eTag::END_NAME) {
            auto item = std::move(stack.top());
            stack.pop();
            InsertItem(stack.top(), std::move(item));
        }
        else {
            stack.push(ConvertToData(token));
        }

        if (!stack.top().IsList() && !stack.top().IsNamed()) {
            auto item = std::move(stack.top());
            stack.pop();
            InsertItem(stack.top(), std::move(item));
        }

    } while (true);

    if (stack.size() != 1) {
        throw std::invalid_argument("archive terminated improperly");
    }

    stream = std::move(stack.top());
}