#pragma once

#include "Native.hpp"
#include "Value.hpp"
#include <async++/task.hpp>


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
    UID invokingId;
    UID methodId;
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
Value MethodResultToValue(const MethodResult& result);
void MethodStatusToException(std::string_view methodName, eMethodStatus status);


struct CallContext {
    UID invokingId;
    std::function<asyncpp::task<MethodResult>(MethodCall)> callRemoteMethod;
    std::function<std::string(UID)> getMethodName = {};
};


namespace impl_method {

    template <class Required, class Optional>
    struct ParameterList;

    template <class... Requireds, class... Optionals>
    struct ParameterList<std::tuple<Requireds...>, std::tuple<Optionals...>> {
        using Unpacked = std::tuple<typename Requireds::Native..., typename Optionals::Native...>;

        static std::vector<Value> Pack(const typename Requireds::Native&... requiredIns,
                                       const typename Optionals::Native&... optionalIns) {
            std::vector<Value> args;
            (..., args.push_back(requiredIns));
            (..., PushOptional(args, MakeOptional(Optionals::key, optionalIns)));
            return args;
        }

        static Unpacked Unpack(std::span<const Value> results, std::string_view methodName = "<unknown method>") {
            if (results.size() < sizeof...(Requireds)) {
                throw InvocationError(methodName,
                                      std::format("expected {} required results, got {}", results.size(), sizeof...(Requireds)));
            }
            auto requireds = std::tuple(std::move(results[Requireds::key])...);
            const auto optionalSubset = results.subspan(sizeof...(Requireds));
            std::unordered_map<int, Value> presentOptionals;
            for (auto& result : optionalSubset) {
                if (!result.Is<Named>()) {
                    throw InvocationError(methodName,
                                          std::format("expected named values for optional results, got {}", result.GetTypeStr()));
                }
                auto [name, value] = std::move(result.Get<Named>());
                if (!name.IsInteger()) {
                    throw InvocationError(methodName,
                                          std::format("expected named value with integer key, key was {}", name.GetTypeStr()));
                }
                presentOptionals.insert_or_assign(name.Get<int>(), std::move(value));
            }
            auto optionals = std::tuple((std::move(presentOptionals.contains(Optionals::key)
                                                       ? std::optional(std::move(presentOptionals.find(Optionals::key)->second))
                                                       : std::nullopt))...);
            return std::tuple_cat(std::move(requireds), std::move(optionals));
        }

    private:
        static std::optional<Value> MakeOptional(int key, std::optional<Value> value) {
            if (value) {
                return Value(Named{ uint16_t(key), *value });
            }
            return std::nullopt;
        }

        static void PushOptional(std::vector<Value>& out, std::optional<Value> value) {
            if (value) {
                out.push_back(std::move(*value));
            }
        }
    };


    template <UID MethodId,
              class RequiredParams,
              class OptionalParams,
              class RequiredResults,
              class OptionalResults>
    class Method;

    template <UID MethodId,
              class... RequiredParams,
              class... OptionalParams,
              class... RequiredResults,
              class... OptionalResults>
    class Method<MethodId,
                 std::tuple<RequiredParams...>,
                 std::tuple<OptionalParams...>,
                 std::tuple<RequiredResults...>,
                 std::tuple<OptionalResults...>> {
        using InputList = ParameterList<std::tuple<RequiredParams...>, std::tuple<OptionalParams...>>;
        using OutputList = ParameterList<std::tuple<RequiredResults...>, std::tuple<OptionalResults...>>;
        using ResultType = typename OutputList::Unpacked;

    public:
        asyncpp::task<ResultType> operator()(const CallContext& context,
                                             const typename RequiredParams::Native&... requiredIns,
                                             const typename OptionalParams::Native&... optionalIns) const {
            std::vector<Value> args = InputList::Pack(requiredIns..., optionalIns...);

            MethodCall call{
                .invokingId = context.invokingId,
                .methodId = MethodId,
                .args = std::move(args),
                .status = eMethodStatus::SUCCESS,
            };
            MethodResult result = co_await context.callRemoteMethod(std::move(call));

            const std::string methodName = context.getMethodName ? context.getMethodName(MethodId) : "<method name unspecified>";
            if (result.status != eMethodStatus::SUCCESS) {
                MethodStatusToException(methodName, result.status);
            }

            co_return OutputList::Unpack(result.values, methodName);
        }

        template <class Executor>
        std::pair<std::vector<Value>, eMethodStatus> Execute(Executor&& executor,
                                                             std::span<const Value> args) const {
            const auto input = InputList::Unpack(args);
            const auto [result, status] = std::apply(executor, input);
            return { std::apply(&OutputList::Pack, result), status };
        }
    };


    template <int Key, bool Optional>
    struct MethodPackElement {
        static constexpr int key = Key;
        using Native = std::conditional_t<Optional, std::optional<Value>, Value>;
    };


    template <class Keys, bool Optional>
    struct MakeMethodPackHelper;

    template <int... Keys, bool Optional>
    struct MakeMethodPackHelper<std::integer_sequence<int, Keys...>, Optional> {
        using type = std::tuple<MethodPackElement<Keys, Optional>...>;
    };


    template <int Count, bool Optional>
    struct MakeMethodPack {
        using type = MakeMethodPackHelper<std::make_integer_sequence<int, Count>, Optional>::type;
    };

} // namespace impl_method


template <UID MethodId, int RequiredParams, int OptionalParams, int RequiredResults, int OptionalResults>
class Method : public impl_method::Method<MethodId,
                                          typename impl_method::MakeMethodPack<RequiredParams, false>::type,
                                          typename impl_method::MakeMethodPack<OptionalParams, true>::type,
                                          typename impl_method::MakeMethodPack<RequiredResults, false>::type,
                                          typename impl_method::MakeMethodPack<OptionalResults, true>::type> {
public:
    static constexpr auto GetMethodId() {
        return MethodId;
    }
};

} // namespace sedmgr