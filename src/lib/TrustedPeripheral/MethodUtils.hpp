#pragma once

#include "TrustedPeripheral.hpp"

#include <Messaging/Method.hpp>
#include <Messaging/Native.hpp>

#include <asyncpp/task.hpp>


namespace sedmgr {

asyncpp::task<Value> SendPacketizedValue(std::shared_ptr<TrustedPeripheral> tper,
                                         uint8_t protocol,
                                         uint32_t tperSessionNumber,
                                         uint32_t hostSessionNumber,
                                         Value request,
                                         bool isRequestList);

asyncpp::task<MethodResult> CallRemoteMethod(std::shared_ptr<TrustedPeripheral> tper,
                                             uint8_t protocol,
                                             uint32_t tperSessionNumber,
                                             uint32_t hostSessionNumber,
                                             MethodCall call);

asyncpp::task<MethodResult> CallRemoteSessionMethod(std::shared_ptr<TrustedPeripheral> tper,
                                                    uint8_t protocol,
                                                    uint32_t tperSessionNumber,
                                                    uint32_t hostSessionNumber,
                                                    MethodCall call);


template <class T>
Value ConvertArg(const T& arg) {
    return value_cast(arg);
}

template <class T>
std::optional<Value> ConvertArg(const std::optional<T> arg) {
    return arg ? std::optional(value_cast(*arg)) : std::nullopt;
}


template <class T>
struct ConvertResult;


template <>
struct ConvertResult<Value> {
    ConvertResult(Value value) : value(value) {}
    template <class U>
    operator U() const {
        return value_cast<U>(value);
    }
    Value value;
};


template <>
struct ConvertResult<std::optional<Value>> {
    ConvertResult(std::optional<Value> value) : value(value) {}
    template <class U>
    operator std::optional<U>() const {
        return value ? std::optional(value_cast<U>(*value)) : std::nullopt;
    }
    std::optional<Value> value;
};

template <class T>
ConvertResult(T) -> ConvertResult<T>;


template <class T, class... Results>
T ResultAs(const std::tuple<Results...>& results) {
    return std::apply([](const auto&... values) { return T{ ConvertResult(values)... }; }, results);
}


} // namespace sedmgr