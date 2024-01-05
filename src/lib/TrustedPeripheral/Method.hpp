#pragma once

#include <async++/task.hpp>

#include <Archive/Types/ValueToNative.hpp>
#include <Messaging/Method.hpp>
#include <Messaging/NativeTypes.hpp>

#include <algorithm>


namespace sedmgr {

struct CallContext {
    uint64_t invokingId;
    std::function<asyncpp::task<MethodResult>(MethodCall)> callRemoteProcedure;
    std::function<std::string(uint64_t)> getMethodName;
};

namespace impl {
    template <uint64_t MethodId,
              class RequiredParams,
              class OptionalParams,
              class RequiredResults,
              class OptionalResults>
    class Method;

    template <uint64_t MethodId,
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
        asyncpp::task<Result> operator()(CallContext context,
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

            co_return ConstructResults(result.values);
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

        Result ConstructResults(std::span<Value> results) {
            if (results.size() < sizeof...(RequiredResults)) {
                throw InvocationError("TODO:METHOD", std::format("expected {} required results, got {}", results.size(), sizeof...(RequiredResults)));
            }
            auto requireds = std::tuple(std::move(results[RequiredResults::key])...);
            const auto optionalSubset = results.subspan(sizeof...(RequiredResults));
            std::unordered_map<int, Value> presentOptionals;
            for (auto& result : optionalSubset) {
                if (!result.Is<Named>()) {
                    throw InvocationError("TODO:METHOD", std::format("expected named values for optional results, got {}", result.GetTypeStr()));
                }
                auto [name, value] = std::move(result.Get<Named>());
                if (!name.IsInteger()) {
                    throw InvocationError("TODO:METHOD", std::format("expected named value with integer key, key was {}", name.GetTypeStr()));
                }
                presentOptionals.insert_or_assign(name.Get<int>(), std::move(value));
            }
            auto optionals = std::tuple((std::move(presentOptionals.contains(OptionalResults::key)
                                                       ? std::optional(std::move(presentOptionals.find(OptionalResults::key)->second))
                                                       : std::nullopt))...);
            return std::tuple_cat(std::move(requireds), std::move(optionals));
        }
    };
} // namespace impl


template <uint64_t MethodId, int RequiredParams, int OptionalParams, int RequiredResults, int OptionalResults>
class Method : public impl::Method<MethodId, std::tuple<>, std::tuple<>, std::tuple<>, std::tuple<>> {
public:
};


namespace impl {

    // clang-format off
    template <class T>
    concept OptionalType =
        requires(T& v) {
            typename T::value_type;
            { v } -> std::same_as<std::optional<typename T::value_type>&>;
        };
    // clang-format on

    static_assert(OptionalType<std::optional<bool>>);
    static_assert(!OptionalType<float>);


    inline auto LabelOptionalArgs(size_t label) {
        return std::tuple{};
    }


    template <class Arg, class... Args>
    auto LabelOptionalArgs(intptr_t label, const Arg& arg, const Args&... args) {
        constexpr bool isOptional = OptionalType<Arg>;
        if (!isOptional && label != 0) {
            throw std::invalid_argument("optional arguments must follow mandatory arguments");
        }
        const intptr_t currentLabel = isOptional ? label : -1;
        const intptr_t nextLabel = label + intptr_t(isOptional);
        return std::tuple_cat(std::tuple{ currentLabel }, LabelOptionalArgs(nextLabel, args...));
    }


    template <size_t Element, std::same_as<intptr_t>... Labels, std::same_as<Value>... Streams>
    std::vector<Value> CollapseArgs(std::tuple<Labels...> labels, std::tuple<Streams...> streams) {
        if constexpr (Element >= sizeof...(Labels)) {
            return {};
        }
        else {
            const auto label = std::get<Element>(labels);
            auto stream = std::move(std::get<Element>(streams));
            assert(label >= 0 || stream.HasValue());

            auto rest = CollapseArgs<Element + 1>(labels, std::move(streams));

            if (label >= 0 && stream.HasValue()) {
                rest.emplace_back(Named(uint32_t(label), std::move(stream)));
            }
            else {
                rest.push_back(std::move(stream));
            }
            return rest;
        }
    }


    std::vector<std::pair<intptr_t, const Value&>> LabelOptionalArgs(std::span<const Value> streams);


    template <size_t Element, std::same_as<intptr_t>... Labels, class... Args>
    void ExpandArgs(std::vector<std::pair<intptr_t, const Value&>> labeledStreams,
                    std::tuple<Labels...> labels,
                    std::tuple<Args&...> args) {
        if constexpr (Element < sizeof...(Args)) {
            auto& arg = std::get<Element>(args);
            const auto label = std::get<Element>(labels);

            if constexpr (OptionalType<std::decay_t<decltype(arg)>>) {
                auto it = std::ranges::find_if(labeledStreams, [label](auto& item) { return item.first == label; });
                if (it != labeledStreams.end()) {
                    arg = value_cast<typename std::decay_t<decltype(arg)>::value_type>(it->second);
                }
            }
            else {
                if (!(Element < labeledStreams.size())) {
                    throw std::invalid_argument(std::format("expected at least {} arguments, got {}", Element + 1, labeledStreams.size()));
                }
                auto&& [dynLabel, stream] = labeledStreams[Element];
                if (dynLabel != -1) {
                    throw std::invalid_argument(std::format("expected mandatory argument as argument {}", Element));
                }
                arg = value_cast<std::decay_t<decltype(arg)>>(stream);
            }

            ExpandArgs<Element + 1>(labeledStreams, labels, args);
        }
    }
} // namespace impl


template <class... Args>
std::vector<Value> ArgsToValues(const Args&... args) {
    const auto labels = impl::LabelOptionalArgs(0, args...);
    const auto streams = std::tuple{ value_cast(args)... };
    auto list = impl::CollapseArgs<0>(labels, std::move(streams));
    std::ranges::reverse(list);
    return list;
}


template <class... Args>
void ArgsFromValues(std::span<const Value> streams, Args&... args) {
    const auto labels = impl::LabelOptionalArgs(0, args...);
    const auto labeledStreams = impl::LabelOptionalArgs(streams);
    impl::ExpandArgs<0>(labeledStreams, labels, std::tie(args...));
}

} // namespace sedmgr