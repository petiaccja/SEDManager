#pragma once


#include "../FlatBinaryArchive.hpp"
#include "../Token.hpp"

#include <Data/Value.hpp>

#include <cereal/cereal.hpp>

#include <cassert>
#include <stack>


namespace sedmgr {

// TODO: return a coroutine generator
std::vector<Token> ValueToTokens(const Value& value);

// TODO: accept a coroutine generator
Value TokensToValue(std::span<const Token> tokens);


template <class Archive>
void save(Archive& ar, const Value& value) {
    const auto tokens = ValueToTokens(value);
    for (const auto& token : tokens) {
        ar(token);
    }
}


template <class Archive>
void save_strip_list(Archive& ar, const Value& value) {
    if (value.IsList()) {
        for (const auto& item : value.GetList()) {
            save(ar, item);
        }
    }
    else {
        save(ar, value);
    }
}


template <class Archive>
void load(Archive& ar, Value& value) {
    std::vector<Token> tokens;
    try {
        while (true) {
            Token token;
            ar(token);
            tokens.push_back(std::move(token));
        }
    }
    catch (std::exception& ex) {
        // No other way to detect end of archive? Would be nice.
    }
    value = TokensToValue(tokens);
}

} // namespace sedmgr