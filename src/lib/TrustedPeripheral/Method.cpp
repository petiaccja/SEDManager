#include "Method.hpp"

#include <Archive/Conversion.hpp>
#include <Error/Exception.hpp>


namespace sedmgr {

Value MethodToValue(Uid invokingId, const Method& method) {
    Value stream = {
        eCommand::CALL,
        ToBytes(uint64_t(invokingId)),
        ToBytes(uint64_t(method.methodId)),
        method.args,
        eCommand::END_OF_DATA,
        {uint8_t(method.status), uint8_t(0), uint8_t(0)},
    };
    return stream;
}


Method MethodFromValue(const Value& stream) {
    if (!stream.IsList()) {
        throw std::invalid_argument("expected a list as top-level item");
    }
    const auto content = stream.Get<std::span<const Value>>();
    if (content.size() < 6) {
        throw std::invalid_argument("method stream must contains at least CALL, invoking ID, method ID, arg list, EOD, and status list");
    }
    try {
        const auto call = content[0].Get<eCommand>();
        const auto invokingIdBytes = content[1].Get<std::span<const std::byte>>();
        const auto methodIdBytes = content[2].Get<std::span<const std::byte>>();
        const auto args = content[3].Get<std::span<const Value>>();
        const auto eod = content[4].Get<eCommand>();
        const auto statusList = content[5].Get<std::span<const Value>>();

        if (call != eCommand::CALL) {
            throw std::invalid_argument("expected a leading call token");
        }
        if (eod != eCommand::END_OF_DATA) {
            throw std::invalid_argument("expected an end of data token after arg list");
        }
        if (statusList.size() != 3) {
            throw std::invalid_argument("status list must have 3 elements");
        }
        const auto statusCode = statusList[0].Get<unsigned>();

        uint64_t methodId;
        FromBytes(methodIdBytes, methodId);
        Method method{
            .methodId = methodId,
            .args = {begin(args), end(args)},
            .status = static_cast<eMethodStatus>(statusCode),
        };
        return method;
    }
    catch (std::exception&) {
        throw std::invalid_argument("incorrect method format");
    }
}


MethodResult MethodResultFromValue(const Value& result) {
    if (!result.IsList()) {
        throw std::invalid_argument("expected a list as top-level item");
    }
    const auto content = result.Get<std::span<const Value>>();
    if (content.size() < 3) {
        throw std::invalid_argument("method result stream must contains at least result list, EOD, and status list");
    }

    if (content[0].IsCommand() && content[0].Get<eCommand>() == eCommand::CALL) {
        Method method;
        try {
            method = MethodFromValue(result);
        }
        catch (std::exception& ex) {
            throw std::invalid_argument(std::format("failed to parse results (result was a call): {}", ex.what()));
        }
        if (uint64_t(method.methodId) == 0xFF06) {
            throw InvocationError("CloseSession", "received CloseSession as response, session terminated by TPer");
        }
    }

    try {
        const auto results = content[0].Get<std::span<const Value>>();
        const auto eod = content[1].Get<eCommand>();
        const auto statusList = content[2].Get<std::span<const Value>>();

        if (eod != eCommand::END_OF_DATA) {
            throw std::invalid_argument("expected an end of data token after result list");
        }
        if (statusList.size() != 3) {
            throw std::invalid_argument("status list must have 3 elements");
        }
        const auto statusCode = statusList[0].Get<unsigned>();

        MethodResult methodResult{
            .values = {begin(results), end(results)},
            .status = static_cast<eMethodStatus>(statusCode),
        };
        return methodResult;
    }
    catch (std::exception& ex) {
        throw std::invalid_argument(std::format("failed to parse results: {}", ex.what()));
    }
}


void MethodStatusToException(std::string_view methodName, eMethodStatus status) {
    switch (status) {
        case eMethodStatus::NOT_AUTHORIZED: throw NotAuthorizedError(methodName);
        case eMethodStatus::SP_BUSY: throw SecurityProviderBusyError(methodName);
        case eMethodStatus::SP_FAILED: throw SecurityProviderFailedError(methodName);
        case eMethodStatus::SP_DISABLED: throw SecurityProviderDisabledError(methodName);
        case eMethodStatus::SP_FROZEN: throw SecurityProviderFrozenError(methodName);
        case eMethodStatus::NO_SESSIONS_AVAILABLE: throw NoSessionsAvailableError(methodName);
        case eMethodStatus::UNIQUENESS_CONFLICT: throw UniquenessConflictError(methodName);
        case eMethodStatus::INSUFFICIENT_SPACE: throw InsufficientSpaceError(methodName);
        case eMethodStatus::INSUFFICIENT_ROWS: throw InsufficientRowsError(methodName);
        case eMethodStatus::INVALID_PARAMETER: throw InvalidParameterError(methodName);
        case eMethodStatus::TPER_MALFUNCTION: throw TPerMalfunctionError(methodName);
        case eMethodStatus::TRANSACTION_FAILURE: throw TransactionFailureError(methodName);
        case eMethodStatus::RESPONSE_OVERFLOW: throw ResponseOverflowError(methodName);
        case eMethodStatus::AUTHORITY_LOCKED_OUT: throw AuthorityLockedOutError(methodName);
        case eMethodStatus::OBSOLETE: [[fallthrough]];
        case eMethodStatus::OBSOLETE_1: [[fallthrough]];
        case eMethodStatus::OBSOLETE_2: [[fallthrough]];
        case eMethodStatus::FAIL: throw InvocationError(methodName, "unknown error");
        default: return;
    }
}


namespace impl {

    std::vector<std::pair<intptr_t, const Value&>> LabelOptionalArgs(std::span<const Value> streams) {
        using Item = std::pair<intptr_t, const Value&>;
        std::vector<Item> labels;
        std::ranges::transform(streams, std::back_inserter(labels), [](const Value& stream) {
            if (stream.IsNamed()) {
                const auto& named = stream.Get<Named>();
                if (!named.name.IsInteger()) {
                    throw std::invalid_argument("expected an integer as argument label");
                }
                return Item{ named.name.Get<intptr_t>(), named.value };
            }
            return Item{ intptr_t(-1), stream };
        });
        return labels;
    }

} // namespace impl

} // namespace sedmgr