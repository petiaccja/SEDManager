#pragma once

#include <format>
#include <optional>
#include <stdexcept>


namespace sedmgr {

//------------------------------------------------------------------------------
// General errors
//------------------------------------------------------------------------------

struct NotImplementedError : std::logic_error {
    NotImplementedError(std::string_view message = {})
        : std::logic_error(message.empty() ? std::string("not implemented") : std::format("not implemented: {}", message)) {}
};


struct PasswordError : std::logic_error {
    PasswordError() : std::logic_error("invalid password") {}
};


//------------------------------------------------------------------------------
// Data format and data type related errors
//------------------------------------------------------------------------------

struct InvalidTypeError : std::logic_error {
    InvalidTypeError(std::string_view message)
        : std::logic_error(std::format("invalid type: {}", message)) {}
};


struct UnexpectedTypeError : InvalidTypeError {
    UnexpectedTypeError(std::string_view expected, std::optional<std::string_view> actual = {}, std::optional<std::string_view> message = {})
        : InvalidTypeError(Message(expected, actual, message)) {}

    static std::string Message(std::string_view expected, std::optional<std::string_view> actual = {}, std::optional<std::string_view> message = {}) {
        std::string expectedBit = std::format("expected a value of type '{}'", expected);
        std::string actualBit = actual ? std::format("but got a value of type '{}'", *actual) : std::string{};
        std::string messageBit = message ? std::format(": {}", *message) : std::string{};
        return expectedBit + actualBit + messageBit;
    }
};


struct TypeConversionError : std::logic_error {
    TypeConversionError(std::string_view source, std::string_view target)
        : std::logic_error(std::format("could not convert '{}' to '{}'", source, target)) {}
};


struct InvalidFormatError : std::logic_error {
    InvalidFormatError(std::string_view message)
        : std::logic_error(std::format("invalid format: {}", message)) {}
};


//------------------------------------------------------------------------------
// RPC errors
//------------------------------------------------------------------------------


struct DeviceError : std::runtime_error {
    DeviceError(std::string message) : std::runtime_error(std::move(message)) {}
};


struct ProtocolError : std::logic_error {
    ProtocolError(std::string message) : std::logic_error(std::move(message)) {}
};


struct InvalidResponseError : std::logic_error {
    InvalidResponseError(std::string_view message)
        : std::logic_error(std::format("invalid response: {}", message)) {}
    InvalidResponseError(std::string_view methodName, std::string_view message)
        : std::logic_error(std::format("invalid response to '{}': {}", methodName, message)) {}
};


struct NoResponseError : std::logic_error {
    NoResponseError()
        : std::logic_error("no response") {}
    NoResponseError(std::string_view message)
        : std::logic_error(std::format("no response: {}", message)) {}
};


struct InvocationError : std::logic_error {
    InvocationError(std::string_view methodName, std::string_view message)
        : std::logic_error(std::format("invoking '{}' failed: {}", methodName, message)) {}
};

struct NotAuthorizedError : InvocationError {
    NotAuthorizedError(std::string_view methodName) : InvocationError(methodName, "not authorized") {}
};
struct SecurityProviderBusyError : InvocationError {
    SecurityProviderBusyError(std::string_view methodName) : InvocationError(methodName, "security provider busy") {}
};
struct SecurityProviderFailedError : InvocationError {
    SecurityProviderFailedError(std::string_view methodName) : InvocationError(methodName, "security provider failed") {}
};
struct SecurityProviderDisabledError : InvocationError {
    SecurityProviderDisabledError(std::string_view methodName) : InvocationError(methodName, "security provider disabled") {}
};
struct SecurityProviderFrozenError : InvocationError {
    SecurityProviderFrozenError(std::string_view methodName) : InvocationError(methodName, "security provider frozen") {}
};
struct NoSessionsAvailableError : InvocationError {
    NoSessionsAvailableError(std::string_view methodName) : InvocationError(methodName, "no session available") {}
};
struct UniquenessConflictError : InvocationError {
    UniquenessConflictError(std::string_view methodName) : InvocationError(methodName, "uniqueness conflict") {}
};
struct InsufficientSpaceError : InvocationError {
    InsufficientSpaceError(std::string_view methodName) : InvocationError(methodName, "insufficient space") {}
};
struct InsufficientRowsError : InvocationError {
    InsufficientRowsError(std::string_view methodName) : InvocationError(methodName, "insufficient rows") {}
};
struct InvalidParameterError : InvocationError {
    InvalidParameterError(std::string_view methodName) : InvocationError(methodName, "invalid parameter") {}
};
struct TPerMalfunctionError : InvocationError {
    TPerMalfunctionError(std::string_view methodName) : InvocationError(methodName, "TPer malfunction") {}
};
struct TransactionFailureError : InvocationError {
    TransactionFailureError(std::string_view methodName) : InvocationError(methodName, "transaction failure") {}
};
struct ResponseOverflowError : InvocationError {
    ResponseOverflowError(std::string_view methodName) : InvocationError(methodName, "response overflow") {}
};
struct AuthorityLockedOutError : InvocationError {
    AuthorityLockedOutError(std::string_view methodName) : InvocationError(methodName, "authority locked out") {}
};

} // namespace sedmgr