#pragma once

#include <Error/Exception.hpp>

#include <any>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <format>
#include <memory>
#include <span>
#include <string>
#include <typeinfo>
#include <variant>
#include <vector>


namespace sedmgr {

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
    using List = std::vector<Value>;
    using Bytes = std::vector<std::byte>;
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
    auto VisitInt(Visitor&& visitor) const;

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
    bool HasValue() const;
    const std::type_info& Type() const;
    std::string GetTypeStr() const;

    bool operator==(const Value& rhs) const;
    bool operator!=(const Value& rhs) const { return !operator==(rhs); }

private:
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
auto Value::VisitInt(Visitor&& visitor) const {
    if (!IsInteger()) {
        throw TypeConversionError(GetTypeStr(), "<int*>");
    }
    const auto _visitor = [&]<class S>(const S& value) -> std::invoke_result_t<Visitor, int> {
        if constexpr (std::is_integral_v<S>) {
            return visitor(value);
        }
        throw TypeConversionError(GetTypeStr(), "<int*>");
    };
    return std::visit(_visitor, *m_storage);
}


template <std::integral T>
T Value::GetInt() const {
    assert(m_storage);
    const auto visitor = [this]<class S>(const S& value) -> T {
        if constexpr (std::is_integral_v<S>) {
            return static_cast<T>(value);
        }
        else {
            const auto targetType = std::format("{}int{}", std::is_signed_v<T> ? "" : "u", sizeof(T) * 8);
            const auto actualType = GetTypeStr();
            throw TypeConversionError(actualType, targetType);
        }
    };
    return std::visit(visitor, *m_storage);
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


} // namespace sedmgr