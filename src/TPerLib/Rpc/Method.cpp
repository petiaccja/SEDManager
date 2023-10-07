#include "Method.hpp"

#include "../Serialization/Utility.hpp"


std::string_view MethodStatusText(eMethodStatus status) {
    switch (status) {
        case eMethodStatus::SUCCESS: return "success";
        case eMethodStatus::NOT_AUTHORIZED: return "not authorized";
        case eMethodStatus::OBSOLETE: return "obsolete";
        case eMethodStatus::SP_BUSY: return "security provider busy";
        case eMethodStatus::SP_FAILED: return "security provider failed";
        case eMethodStatus::SP_DISABLED: return "security provider disabled";
        case eMethodStatus::SP_FROZEN: return "security provider frozen";
        case eMethodStatus::NO_SESSIONS_AVAILABLE: return "no sessions available";
        case eMethodStatus::UNIQUENESS_CONFLICT: return "uniqueness conflict";
        case eMethodStatus::INSUFFICIENT_SPACE: return "insufficient space";
        case eMethodStatus::INSUFFICIENT_ROWS: return "insufficient rows";
        case eMethodStatus::INVALID_PARAMETER: return "invalid argument";
        case eMethodStatus::OBSOLETE_1: return "obsolete 1";
        case eMethodStatus::OBSOLETE_2: return "obsolete 2";
        case eMethodStatus::TPER_MALFUNCTION: return "trusted peripheral malfunction";
        case eMethodStatus::TRANSACTION_FAILURE: return "transaction failure";
        case eMethodStatus::RESPONSE_OVERFLOW: return "response overflow";
        case eMethodStatus::AUTHORITY_LOCKED_OUT: return "authority locked out";
        case eMethodStatus::FAIL: return "undetermined failure";
        default: return "unrecognized status code";
    }
}


TokenStream SerializeMethod(Uid invokingId, const Method& method) {
    TokenStream stream = {
        eCommand::CALL,
        { TokenStream::bytes, ToBytes(uint64_t(invokingId)) },
        { TokenStream::bytes, ToBytes(uint64_t(method.methodId)) },
        method.args,
        eCommand::END_OF_DATA,
        { uint8_t(method.status), uint8_t(0), uint8_t(0) },
    };
    return stream;
}


Method ParseMethod(const TokenStream& stream) {
    if (!stream.IsList()) {
        throw std::invalid_argument("expected a list as top-level item");
    }
    const auto content = stream.Get<std::span<const TokenStream>>();
    if (content.size() < 6) {
        throw std::invalid_argument("method stream must contains at least CALL, invoking ID, method ID, arg list, EOD, and status list");
    }
    try {
        const auto call = content[0].Get<eCommand>();
        const auto invokingIdBytes = content[1].Get<std::span<const uint8_t>>();
        const auto methodIdBytes = content[2].Get<std::span<const uint8_t>>();
        const auto args = content[3].Get<std::span<const TokenStream>>();
        const auto eod = content[4].Get<eCommand>();
        const auto statusList = content[5].Get<std::span<const TokenStream>>();

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
            .args = { begin(args), end(args) },
            .status = static_cast<eMethodStatus>(statusCode),
        };
        return method;
    }
    catch (std::exception&) {
        throw std::invalid_argument("incorrect method format");
    }
}


MethodResult ParseMethodResult(const TokenStream& stream) {
    if (!stream.IsList()) {
        throw std::invalid_argument("expected a list as top-level item");
    }
    const auto content = stream.Get<std::span<const TokenStream>>();
    if (content.size() < 3) {
        throw std::invalid_argument("method result stream must contains at least result list, EOD, and status list");
    }
    try {
        const auto results = content[0].Get<std::span<const TokenStream>>();
        const auto eod = content[1].Get<eCommand>();
        const auto statusList = content[2].Get<std::span<const TokenStream>>();

        if (eod != eCommand::END_OF_DATA) {
            throw std::invalid_argument("expected an end of data token after result list");
        }
        if (statusList.size() != 3) {
            throw std::invalid_argument("status list must have 3 elements");
        }
        const auto statusCode = statusList[0].Get<unsigned>();

        MethodResult methodResult{
            .values = { begin(results), end(results) },
            .status = static_cast<eMethodStatus>(statusCode),
        };
        return methodResult;
    }
    catch (std::exception&) {
        throw std::invalid_argument("incorrect method result format");
    }
}


namespace impl {

std::vector<std::pair<intptr_t, const TokenStream&>> LabelOptionalArgs(std::span<const TokenStream> streams) {
    using Item = std::pair<intptr_t, const TokenStream&>;
    std::vector<Item> labels;
    std::ranges::transform(streams, std::back_inserter(labels), [](const TokenStream& stream) {
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