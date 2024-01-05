#pragma once

#include "Value.hpp"
#include "NativeTypes.hpp"


namespace sedmgr {

enum class eMethodStatus : uint8_t {
    SUCCESS = 0x00,
    NOT_AUTHORIZED = 0x01,
    OBSOLETE = 0x02,
    SP_BUSY = 0x03,
    SP_FAILED = 0x04,
    SP_DISABLED = 0x05,
    SP_FROZEN = 0x06,
    NO_SESSIONS_AVAILABLE = 0x07,
    UNIQUENESS_CONFLICT = 0x08,
    INSUFFICIENT_SPACE = 0x09,
    INSUFFICIENT_ROWS = 0x0A,
    INVALID_PARAMETER = 0x0C,
    OBSOLETE_1 = 0x0D,
    OBSOLETE_2 = 0x0E,
    TPER_MALFUNCTION = 0x0F,
    TRANSACTION_FAILURE = 0x10,
    RESPONSE_OVERFLOW = 0x11,
    AUTHORITY_LOCKED_OUT = 0x12,
    FAIL = 0x3F,
};


struct MethodCall {
    Uid invokingId;
    Uid methodId;
    std::vector<Value> args;
    eMethodStatus status = eMethodStatus::SUCCESS;
};


struct MethodResult {
    std::vector<Value> values;
    eMethodStatus status = eMethodStatus::SUCCESS;
};

Value MethodCallToValue(const MethodCall& method);
MethodCall MethodCallFromValue(const Value& value);
MethodResult MethodResultFromValue(const Value& value);
void MethodStatusToException(std::string_view methodName, eMethodStatus status);

}