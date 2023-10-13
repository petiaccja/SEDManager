#pragma once

#include <format>
#include <stdexcept>


struct MethodError : std::exception {
    MethodError(std::string message) : m_message(std::move(message)) {}

    const char* what() const noexcept override { return m_message.data(); }

private:
    std::string m_message;
};


struct InvocationError : MethodError {
    InvocationError(std::string_view methodName, std::string_view message)
        : MethodError(std::format("invoking '{}' failed: {}", methodName, message)) {}
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


struct InvalidResponseError : MethodError {
    InvalidResponseError(std::string_view message)
        : MethodError(std::format("invalid response: {}", message)) {}
    InvalidResponseError(std::string_view methodName, std::string_view message)
        : MethodError(std::format("invalid response to '{}': {}", methodName, message)) {}
};


struct NoResponseError : MethodError {
    NoResponseError()
        : MethodError("no response") {}
    NoResponseError(std::string_view methodName)
        : MethodError(std::format("no response to '{}'", methodName)) {}
};