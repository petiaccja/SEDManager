#pragma once

#include "../Serialization/TokenStreamArchive.hpp"

#include <concepts>
#include <cstdint>
#include <ranges>
#include <span>


struct ControlToken {};

struct StartList : ControlToken {
    static constexpr uint8_t Header() { return 0xF0u; }
};

struct EndList : ControlToken {
    static constexpr uint8_t Header() { return 0xF1u; }
};

struct StartName : ControlToken {
    static constexpr uint8_t Header() { return 0xF2u; }
};

struct EndName : ControlToken {
    static constexpr uint8_t Header() { return 0xF3u; }
};

struct Call : ControlToken {
    static constexpr uint8_t Header() { return 0xF8u; }
};

struct EndOfData : ControlToken {
    static constexpr uint8_t Header() { return 0xF9u; }
};

struct EndOfSession : ControlToken {
    static constexpr uint8_t Header() { return 0xFAu; }
};

struct StartTransaction : ControlToken {
    static constexpr uint8_t Header() { return 0xFBu; }
};

struct EndTransaction : ControlToken {
    static constexpr uint8_t Header() { return 0xFCu; }
};

struct Empty : ControlToken {
    static constexpr uint8_t Header() { return 0xFFu; }
};

template <class Value>
struct Named {
    std::string name;
    Value value;
};

template <class S, class Value>
Named(S, Value) -> Named<Value>;


template <class... Values>
struct List {
    List() = default;
    template <class... Us>
    List(Us&&... values) : values{ std::forward<Us>(values)... } {}
    std::tuple<Values...> values;
};

template <class... Values>
List(Values...) -> List<Values...>;


template <class Archive, class TokenType>
    requires std::derived_from<TokenType, ControlToken>
void save(Archive& ar, const TokenType& trivialToken) {
    Token token{ .tag = trivialToken.Header(), .isByte = false, .isSigned = false, .data = {} };
    ar(token);
}


template <class Archive, class TokenType>
    requires std::derived_from<TokenType, ControlToken>
void load(Archive& ar, TokenType trivialToken) {
    Token token;
    ar(token);
    if (token.tag != trivialToken.Header()) {
        throw std::invalid_argument("token tags don't match");
    }
}


template <std::ranges::range CharacterSequence>
    requires std::same_as<std::ranges::range_value_t<CharacterSequence>, char>
             || std::same_as<std::ranges::range_value_t<CharacterSequence>, uint8_t>
void SaveBytes(TokenStreamOutputArchive& ar, const CharacterSequence& obj) {
    const auto bytes = obj | std::views::transform([](auto c) { return std::bit_cast<uint8_t>(c); });
    std::vector<uint8_t> data{ std::begin(bytes), std::end(bytes) };
    const auto tag = GetTag(data.size());
    Token token{ .tag = tag, .isByte = true, .isSigned = false, .data = std::move(data) };
    ar(token);
}


template <std::ranges::sized_range CharacterSequence>
    requires(std::same_as<std::ranges::range_value_t<CharacterSequence>, char>
             || std::same_as<std::ranges::range_value_t<CharacterSequence>, uint8_t>)
void LoadBytes(TokenStreamInputArchive& ar, CharacterSequence& obj) {
    using CharacterType = std::ranges::range_value_t<CharacterSequence>;

    Token token;
    ar(token);
    if (!token.isByte) {
        throw std::invalid_argument("token is not a byte sequence");
    }

    const auto characters = token.data | std::views::transform([](auto c) { return std::bit_cast<CharacterType>(c); });
    constexpr bool resizable = requires(CharacterSequence& s, size_t sz) { s.resize(sz); };
    if constexpr (resizable) {
        obj.resize(token.data.size());
    }
    else {
        if (std::ranges::size(obj) != token.data.size()) {
            throw std::invalid_argument("token has a different length compared to the byte sequence");
        }
    }
    auto it = std::begin(obj);
    for (auto c : characters) {
        *it = c;
    }
}


template <class... Elems>
void save(TokenStreamOutputArchive& ar, const List<Elems...>& obj) {
    const auto unwrap = [&](const auto&... elements) {
        (..., ar(elements));
    };

    ar(StartList{});
    std::apply(unwrap, obj.values);
    ar(EndList{});
}


template <class... Elems>
void load(TokenStreamInputArchive& ar, List<Elems...>& obj) {
    StartList sl;
    EndList el;
    const auto unwrap = [&](auto&... elements) {
        (..., ar(elements));
    };

    ar(sl);
    std::apply(unwrap, obj.values);
    ar(el);
}


template <class Value>
void save(TokenStreamOutputArchive& ar, const Named<Value>& obj) {
    ar(StartName{});
    ar(obj.name);
    ar(obj.value);
    ar(EndName{});
}


template <class Value>
void load(TokenStreamInputArchive& ar, Named<Value>& obj) {
    StartName sn;
    EndName en;

    ar(sn);
    ar(obj.name);
    ar(obj.value);
    ar(en);
}


template <size_t N>
void save(TokenStreamOutputArchive& ar, const std::array<uint8_t, N>& obj) {
    SaveBytes(ar, obj);
}
template <size_t N>
void load(TokenStreamInputArchive& ar, std::array<uint8_t, N>& obj) {
    LoadBytes(ar, obj);
}

inline void save(TokenStreamOutputArchive& ar, const std::string& obj) {
    SaveBytes(ar, obj);
}
inline void load(TokenStreamInputArchive& ar, std::string& obj) {
    LoadBytes(ar, obj);
}

inline void save(TokenStreamOutputArchive& ar, const std::string_view& obj) {
    SaveBytes(ar, obj);
}

inline void save(TokenStreamOutputArchive& ar, const std::vector<uint8_t>& obj) {
    SaveBytes(ar, obj);
}
inline void load(TokenStreamInputArchive& ar, std::vector<uint8_t>& obj) {
    LoadBytes(ar, obj);
}