#pragma once

#include <algorithm>
#include <any>
#include <concepts>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <typeinfo>
#include <vector>



enum class eTag : uint8_t {
    START_LIST = 0xF0,
    END_LIST = 0xF1,
    START_NAME = 0xF2,
    END_NAME = 0xF3,
    CALL = 0xF8,
    END_OF_DATA = 0xF9,
    END_OF_SESSION = 0xFA,
    START_TRANSACTION = 0xFB,
    END_TRANSACTION = 0xFC,
    EMPTY = 0xFF,
};


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


class RpcStream {
    using ListType = std::vector<RpcStream>;
    using BytesType = std::vector<uint8_t>;
    struct AsBytesType {};

public:
    static constexpr auto bytes = AsBytesType{};

public:
    RpcStream() = default;

    // Integers.
    RpcStream(std::integral auto value);
    // Lists.
    RpcStream(std::initializer_list<RpcStream> values);
    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, RpcStream>
    RpcStream(R&& values);
    // Bytes.
    template <std::ranges::range R>
        requires std::is_trivial_v<std::ranges::range_value_t<R>>
    RpcStream(AsBytesType, R&& values);
    // Named.
    RpcStream(std::string_view name, RpcStream value);
    RpcStream(Named value);
    // Commands.
    RpcStream(eCommand command);

    template <std::integral T>
    T Get() const;

    template <class T>
        requires is_const_std_span<std::decay_t<T>>::value
                 && std::is_trivial_v<std::ranges::range_value_t<T>>
    std::decay_t<T> Get() const;

    template <std::same_as<std::span<const RpcStream>> T>
    std::span<const RpcStream> Get() const;

    template <std::same_as<Named> T>
    const Named& Get() const;

    template <std::same_as<eCommand> T>
    eCommand Get() const;

    bool IsInteger();
    bool IsBytes();
    bool IsList();
    bool IsNamed();
    bool IsCommand();

    const std::type_info& Type() const { return m_value.type(); }

    // Integers.
    template <std::integral T>
    T AsInt() const;
    // Lists.
    std::span<const RpcStream> AsList() const;
    // Bytes.
    template <class T>
        requires std::is_trivial_v<T>
    std::span<const T> AsBytes() const;
    // Named.
    const Named& AsNamed() const;
    // Commands.
    eCommand AsCommand() const;

private:
    std::any m_value;
};


struct Named {
    std::string name;
    RpcStream value;
};



//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

// Integers.
RpcStream::RpcStream(std::integral auto value)
    : m_value(value) {}


// Lists.
template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, RpcStream>
RpcStream::RpcStream(R&& values)
    : m_value(ListType(std::ranges::begin(values), std::ranges::end(values))) {}


// Bytes.
template <std::ranges::range R>
    requires std::is_trivial_v<std::ranges::range_value_t<R>>
RpcStream::RpcStream(AsBytesType, R&& values) {
    BytesType bytes;
    for (const auto& v : values) {
        const std::span itemBytes{ reinterpret_cast<const uint8_t*>(&v), sizeof(v) };
        std::ranges::copy(itemBytes, std::back_inserter(bytes));
    }
    m_value = std::move(bytes);
}

//------------------------------------------------------------------------------
// Lookup
//------------------------------------------------------------------------------

template <class ResultT, class TestType, class... TestTypes>
ResultT GetFromAny(const std::any& any) {
    if constexpr (sizeof...(TestTypes) == 0) {
        return std::any_cast<const TestType&>(any);
    }
    else if (typeid(TestType) != any.type()) {
        return GetFromAny<ResultT, TestTypes...>(any);
    }
    return static_cast<ResultT>(std::any_cast<TestType>(any));
}

// Integers.
template <std::integral T>
T RpcStream::AsInt() const {
    return GetFromAny<T, bool, char, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>(m_value);
}

// Bytes.
template <class T>
    requires std::is_trivial_v<T>
std::span<const T> RpcStream::AsBytes() const {
    const auto& bytes = std::any_cast<const BytesType&>(m_value);
    std::span<const T> objects{ reinterpret_cast<const T*>(bytes.data()), bytes.size() / sizeof(T) };
    return objects;
}



template <std::integral T>
T RpcStream::Get() const {
    return AsInt<T>();
}

template <class T>
    requires is_const_std_span<std::decay_t<T>>::value
             && std::is_trivial_v<std::ranges::range_value_t<T>>
std::decay_t<T> RpcStream::Get() const {
    return AsBytes<std::ranges::range_value_t<T>>();
}

template <std::same_as<std::span<const RpcStream>> T>
std::span<const RpcStream> RpcStream::Get() const {
    return AsList();
}

template <std::same_as<Named> T>
const Named& RpcStream::Get() const {
    return AsNamed();
}

template <std::same_as<eCommand> T>
eCommand RpcStream::Get() const {
    return AsCommand();
}