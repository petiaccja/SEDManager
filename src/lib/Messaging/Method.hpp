#pragma once

#include "NativeTypes.hpp"
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


struct CallContext {
    Uid invokingId;
    std::function<asyncpp::task<MethodResult>(const MethodCall&)> callRemoteProcedure;
    std::function<std::string(Uid)> getMethodName = {};
};

namespace impl {
    template <Uid MethodId,
              class RequiredParams,
              class OptionalParams,
              class RequiredResults,
              class OptionalResults>
    class Method;

    template <Uid MethodId,
              class... RequiredParams,
              class... OptionalParams,
              class... RequiredResults,
              class... OptionalResults>
    class Method<MethodId,
                 std::tuple<RequiredParams...>,
                 std::tuple<OptionalParams...>,
                 std::tuple<RequiredResults...>,
                 std::tuple<OptionalResults...>> {
        using Result = std::tuple<typename RequiredResults::Native..., typename OptionalResults::Native...>;

    public:
        asyncpp::task<Result> operator()(const CallContext& context,
                                         const typename RequiredParams::Native&... requiredIns,
                                         const typename OptionalParams::Native&... optionalIns) {
            std::vector<Value> args = ConstructArguments(requiredIns..., optionalIns...);

            const MethodCall call{
                .invokingId = context.invokingId,
                .methodId = MethodId,
                .args = std::move(args),
                .status = eMethodStatus::SUCCESS,
            };
            MethodResult result = co_await context.callRemoteProcedure(call);
            if (result.status != eMethodStatus::SUCCESS) {
                MethodStatusToException(context.getMethodName(MethodId), result.status);
            }

            co_return ConstructResults(context, result.values);
        }

    private:
        std::optional<Value> ConstructOptional(int key, std::optional<Value> value) {
            if (value) {
                return Value(Named{ uint16_t(key), *value });
            }
            return std::nullopt;
        }

        void PushOptional(std::vector<Value>& out, std::optional<Value> value) {
            if (value) {
                out.push_back(std::move(*value));
            }
        }

        std::vector<Value> ConstructArguments(const typename RequiredParams::Native&... requiredIns,
                                              const typename OptionalParams::Native&... optionalIns) {
            std::vector<Value> args;
            (..., args.push_back(requiredIns));
            (..., PushOptional(args, ConstructOptional(OptionalParams::key, optionalIns)));
            return args;
        }

        Result ConstructResults(const CallContext& context, std::span<Value> results) {
            if (results.size() < sizeof...(RequiredResults)) {
                throw InvocationError(context.getMethodName(MethodId),
                                      std::format("expected {} required results, got {}", results.size(), sizeof...(RequiredResults)));
            }
            auto requireds = std::tuple(std::move(results[RequiredResults::key])...);
            const auto optionalSubset = results.subspan(sizeof...(RequiredResults));
            std::unordered_map<int, Value> presentOptionals;
            for (auto& result : optionalSubset) {
                if (!result.Is<Named>()) {
                    throw InvocationError(context.getMethodName(MethodId),
                                          std::format("expected named values for optional results, got {}", result.GetTypeStr()));
                }
                auto [name, value] = std::move(result.Get<Named>());
                if (!name.IsInteger()) {
                    throw InvocationError(context.getMethodName(MethodId),
                                          std::format("expected named value with integer key, key was {}", name.GetTypeStr()));
                }
                presentOptionals.insert_or_assign(name.Get<int>(), std::move(value));
            }
            auto optionals = std::tuple((std::move(presentOptionals.contains(OptionalResults::key)
                                                       ? std::optional(std::move(presentOptionals.find(OptionalResults::key)->second))
                                                       : std::nullopt))...);
            return std::tuple_cat(std::move(requireds), std::move(optionals));
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

} // namespace impl


template <Uid MethodId, int RequiredParams, int OptionalParams, int RequiredResults, int OptionalResults>
class Method : public impl::Method<MethodId,
                                   typename impl::MakeMethodPack<RequiredParams, false>::type,
                                   typename impl::MakeMethodPack<OptionalParams, true>::type,
                                   typename impl::MakeMethodPack<RequiredResults, false>::type,
                                   typename impl::MakeMethodPack<OptionalResults, true>::type> {
public:
    static constexpr auto GetMethodId() {
        return MethodId;
    }
};

} // namespace sedmgr