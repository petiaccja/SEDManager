#pragma once

#include "ValueCast.hpp"
#include "Value.hpp"


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


struct Method {
    Uid methodId;
    std::vector<Value> args;
    eMethodStatus status = eMethodStatus::SUCCESS;
};


struct MethodResult {
    std::vector<Value> values;
    eMethodStatus status = eMethodStatus::SUCCESS;
};

Value MethodToValue(Uid invokingId, const Method& method);
Method MethodFromValue(const Value& stream);
MethodResult MethodResultFromValue(const Value& stream);
void MethodStatusToException(std::string_view methodName, eMethodStatus status);


namespace impl {

template <class Arg>
constexpr bool IsOptional() {
    return std::is_constructible_v<Arg, std::nullopt_t>;
};


inline auto LabelOptionalArgs(size_t label) {
    return std::tuple{};
}


template <class Arg, class... Args>
auto LabelOptionalArgs(intptr_t label, const Arg& arg, const Args&... args) {
    constexpr bool isOptional = IsOptional<Arg>();
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

        if constexpr (IsOptional<std::decay_t<decltype(arg)>>()) {
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