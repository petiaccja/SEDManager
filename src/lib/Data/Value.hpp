#pragma once

#include "Archive/Token.hpp"
#include <Error/Exception.hpp>

#include <algorithm>
#include <any>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <format>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <typeinfo>
#include <variant>
#include <vector>


namespace sedmgr {

class Value;

enum class eCommand : uint8_t {
    CALL = 0xF8,
    END_OF_DATA = 0xF9,
    END_OF_SESSION = 0xFA,
    START_TRANSACTION = 0xFB,
    END_TRANSACTION = 0xFC,
    EMPTY = 0xFF,
};

using Bytes = std::vector<std::byte>;
using List = std::vector<Value>;
struct Named;

namespace impl {
    template <class Type, class Variant>
    struct MatchVariant;

    template <class Type>
    struct MatchVariant<Type, std::variant<>> {
        using Exact = void;
        using Convertible = std::tuple<>;
        static constexpr bool matched = false;
    };

    template <class Type, class Head, class... Rest>
    struct MatchVariant<Type, std::variant<Head, Rest...>> {
        using MyExact = std::conditional_t<std::is_same_v<Type, Head>, Head, void>;
        using Exact = std::conditional_t<!std::is_void_v<MyExact>, MyExact, typename MatchVariant<Type, std::variant<Rest...>>::Exact>;
        using MyConvertible = std::conditional_t<std::is_convertible_v<Type, Head>&& std::is_void_v<MyExact>, std::tuple<Head>, std::tuple<>>;
        using Convertible = decltype(std::tuple_cat(
            std::declval<MyConvertible>(),
            std::declval<typename MatchVariant<Type, std::variant<Rest...>>::Convertible>()));
        static constexpr bool matched = !std::is_void_v<Exact> || std::tuple_size_v<Convertible> > 0;
        static constexpr bool in_place = std::tuple_size_v<Convertible> == 0;
    };
} // namespace impl

class Value {
    using StorageType = std::variant<
        bool,
        char,
        uint8_t,
        uint16_t,
        uint32_t,
        uint64_t,
        int8_t,
        int16_t,
        int32_t,
        int64_t,
        eCommand,
        Named,
        List,
        Bytes>;

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

    template <class Visitor>
    auto Visit(Visitor&& visitor) const;

    template <class T, class Match = impl::MatchVariant<T, StorageType>>
        requires(Match::matched)
    auto Get() -> std::conditional_t<Match::in_place, T&, T>;

    template <class T, class Match = impl::MatchVariant<T, StorageType>>
        requires(Match::matched)
    auto Get() const -> std::conditional_t<Match::in_place, const T&, T>;

    template <class T, class Match = impl::MatchVariant<T, StorageType>>
        requires(!std::is_void_v<typename Match::Exact>)
    bool Is() const;

    bool IsInteger() const;

    bool HasValue() const;

    std::string GetTypeStr() const;

    bool operator==(const Value& rhs) const;
    bool operator!=(const Value& rhs) const { return !operator==(rhs); }

private:
    template <class T, class Match, class... Matches>
    T GetConversion(std::tuple<Match, Matches...>*) const;

    template <class T>
    T GetConversion(std::tuple<>*) const;

    template <class T>
    static std::string GetTypeStr();

private:
    std::shared_ptr<StorageType> m_storage;
};


struct Named {
    Value name;
    Value value;

    bool operator==(const Named&) const = default;
    bool operator!=(const Named&) const = default;
};


Value::Value(std::integral auto value)
    : m_storage(std::make_shared<StorageType>(value)) {}


template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, Value>
Value::Value(R&& values)
    : m_storage(std::make_shared<StorageType>(List(std::ranges::begin(values), std::ranges::end(values)))) {}


template <std::ranges::range R>
    requires std::same_as<std::ranges::range_value_t<R>, std::byte>
Value::Value(R&& values)
    : m_storage(std::make_shared<StorageType>(Bytes(std::ranges::begin(values), std::ranges::end(values)))) {}


template <class Visitor>
auto Value::Visit(Visitor&& visitor) const {
    if (!HasValue()) {
        throw std::invalid_argument("Value is empty");
    }
    return std::visit(visitor, *m_storage);
}


template <class T, class Match>
    requires(Match::matched)
auto Value::Get() -> std::conditional_t<Match::in_place, T&, T> {
    if (!m_storage) {
        throw std::logic_error("value is empty");
    }
    if constexpr (!std::is_void_v<typename Match::Exact>) {
        if (std::holds_alternative<typename Match::Exact>(*m_storage)) {
            return std::get<typename Match::Exact>(*m_storage);
        }
    }
    if constexpr (!Match::in_place) {
        return GetConversion<T>(static_cast<typename Match::Convertible*>(nullptr));
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<T>());
}


template <class T, class Match>
    requires(Match::matched)
auto Value::Get() const -> std::conditional_t<Match::in_place, const T&, T> {
    if (!m_storage) {
        throw std::logic_error("value is empty");
    }
    if constexpr (!std::is_void_v<typename Match::Exact>) {
        if (std::holds_alternative<typename Match::Exact>(*m_storage)) {
            return std::get<typename Match::Exact>(*m_storage);
        }
    }
    if constexpr (!Match::in_place) {
        return GetConversion<T>(static_cast<typename Match::Convertible*>(nullptr));
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<T>());
}


static_assert(std::is_reference_v<decltype(std::declval<Value>().Get<Bytes>())>);
static_assert(std::is_reference_v<decltype(std::declval<Value>().Get<eCommand>())>);
static_assert(std::is_reference_v<decltype(std::declval<Value>().Get<List>())>);
static_assert(std::is_reference_v<decltype(std::declval<Value>().Get<Named>())>);


template <class T, class Match, class... Matches>
T Value::GetConversion(std::tuple<Match, Matches...>*) const {
    assert(m_storage);
    if (std::holds_alternative<Match>(*m_storage)) {
        return static_cast<T>(std::get<Match>(*m_storage));
    }
    return GetConversion<T>(static_cast<std::tuple<Matches...>*>(nullptr));
}

template <class T>
T Value::GetConversion(std::tuple<>*) const {
    throw TypeConversionError(GetTypeStr(), GetTypeStr<T>());
}


template <class T, class Match>
    requires(!std::is_void_v<typename Match::Exact>)
bool Value::Is() const {
    if (!m_storage) {
        return false;
    }
    if (!std::is_void_v<typename Match::Exact>) {
        return std::holds_alternative<typename Match::Exact>(*m_storage);
    }
}


template <class T>
std::string Value::GetTypeStr() {
    if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            return std::format("int{}", 8 * sizeof(T));
        }
        else {
            return std::format("uint{}", 8 * sizeof(T));
        }
    }
    else if constexpr (std::is_same_v<T, List>) {
        return "list";
    }
    else if constexpr (std::is_same_v<T, Named>) {
        return "named";
    }
    else if constexpr (std::is_same_v<T, Bytes>) {
        return "bytes";
    }
    else if constexpr (std::is_same_v<T, eCommand>) {
        return "command";
    }
    else {
        static_assert(sizeof(T) == 0);
        std::terminate();
    }
}


template <std::integral Integral>
std::vector<Token> Tokenize(const Integral& value) {
    const auto tag = Token::GetTag(sizeof(Integral));
    const auto isSigned = std::is_signed_v<Integral>;
    const auto isByte = false;
    auto data = std::vector<std::byte>{};
    for (auto [iter, chunk] = std::tuple(0, value); iter != sizeof(Integral); ++iter, chunk >>= 8) {
        data.push_back(std::byte{ static_cast<uint8_t>(chunk) });
    }
    std::ranges::reverse(data);
    return {
        Token{.tag = tag, .isByte = isByte, .isSigned = isSigned, .data = std::move(data)}
    };
}

template <std::integral Integral>
std::pair<Integral, std::span<const Token>> DeTokenize(Tokenized<Integral> tokens) {
    if (tokens.tokens.empty()) {
        throw std::invalid_argument("expected at least 1 token");
    }
    const Token& myToken = tokens.tokens[0];
    if (!myToken.IsAtom()) {
        throw std::invalid_argument("expected an atom");
    }
    if (myToken.isByte != false) {
        throw std::invalid_argument("expected integer data, got bytes");
    }
    if (myToken.isSigned != std::is_signed_v<Integral>) {
        throw std::invalid_argument("mismatched integer sign");
    }
    if (myToken.data.size() != sizeof(Integral)) {
        throw std::invalid_argument("mismatched integer size");
    }
    std::make_unsigned_t<Integral> value = 0;
    for (const auto byte : myToken.data) {
        value <<= 8;
        value |= static_cast<uint8_t>(byte);
    }
    return { std::bit_cast<Integral>(value), tokens.tokens.subspan(1) };
}

std::vector<Token> Tokenize(const Bytes& value);
std::pair<Bytes, std::span<const Token>> DeTokenize(Tokenized<Bytes> tokens);

std::vector<Token> Tokenize(const eCommand& value);
std::pair<eCommand, std::span<const Token>> DeTokenize(Tokenized<eCommand> tokens);

std::vector<Token> Tokenize(const List& value);
std::pair<List, std::span<const Token>> DeTokenize(Tokenized<List> tokens);

std::vector<Token> Tokenize(const Named& value);
std::pair<Named, std::span<const Token>> DeTokenize(Tokenized<Named> tokens);

std::vector<Token> Tokenize(const Value& value);
std::pair<Value, std::span<const Token>> DeTokenize(Tokenized<Value> tokens);

} // namespace sedmgr