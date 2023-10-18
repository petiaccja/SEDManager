#pragma once


#include "../FlatBinaryArchive.hpp"
#include "../Token.hpp"

#include <Data/Value.hpp>

#include <cereal/cereal.hpp>

#include <cassert>
#include <stack>


template <class Archive>
void SaveInteger(Archive& ar, const Value& value) {
    std::optional<bool> r = ForEachType<Value::IntTypes>([&]<class T>(T* ptr) -> std::optional<bool> {
        if (value.Type() == typeid(T)) {
            const auto bytes = ToFlatBinary(value.Get<T>());
            const Token token{
                .tag = eTag::SHORT_ATOM,
                .isByte = false,
                .isSigned = std::is_signed_v<T>,
                .data = {begin(bytes), end(bytes)},
            };
            ar(token);
            return { true };
        }
        return std::nullopt;
    });
    assert(r);
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
        .data = {bytes.begin(), bytes.end()},
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
        throw InvalidFormatError("invalid token stream");
    }

    stream = std::move(stack.top());
}