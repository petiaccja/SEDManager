#include <async++/join.hpp>

#include <Archive/Types/ValueToNative.hpp>
#include <Messaging/Method.hpp>
#include <Specification/Core/Defs/UIDs.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("Method: parse method - roundtrip", "[Method]") {
    const MethodCall input = {
        .invokingId = 0xFF,
        .methodId = 0xDEADBEEF,
        .args = {1, 2, 3},
        .status = eMethodStatus::FAIL,
    };

    const auto value = MethodCallToValue(input);
    const MethodCall output = MethodCallFromValue(value);

    REQUIRE(output.methodId == input.methodId);
    REQUIRE(output.args.size() == 3);
    REQUIRE(output.args[0].Get<int>() == 1);
    REQUIRE(output.args[1].Get<int>() == 2);
    REQUIRE(output.args[2].Get<int>() == 3);
    REQUIRE(output.status == input.status);
}


TEST_CASE("Method: parse method - invalid input", "[Method]") {
    const Value input = {
        value_cast(Uid(0xFF)),
        eCommand::CALL, // Should be the first in the stream.
        value_cast(Uid(core::eMethod::CloseSession)),
        { 0, 0 },
        eCommand::END_OF_DATA,
        { 0, 0, 0 }
    };
    REQUIRE_THROWS_AS(MethodCallFromValue(input), std::invalid_argument);
}


TEST_CASE("Method: parse results - roundtrip", "[Method]") {
    const Value value = {
        {1,                                          2,          3         },
        eCommand::END_OF_DATA,
        { static_cast<uint8_t>(eMethodStatus::FAIL), uint8_t(0), uint8_t(0)},
    };
    const MethodResult expected = {
        .values = {1, 2, 3},
        .status = eMethodStatus::FAIL,
    };

    const MethodResult output = MethodResultFromValue(value);

    REQUIRE(output.values.size() == 3);
    REQUIRE(output.values[0].Get<int>() == 1);
    REQUIRE(output.values[1].Get<int>() == 2);
    REQUIRE(output.values[2].Get<int>() == 3);
    REQUIRE(output.status == eMethodStatus::FAIL);
}


TEST_CASE("Method: parse results - CloseSession", "[Method]") {
    const Value input = {
        eCommand::CALL,
        value_cast(Uid(0xFF)),
        value_cast(Uid(core::eMethod::CloseSession)),
        { 0, 0 },
        eCommand::END_OF_DATA,
        { 0, 0, 0 }
    };
    REQUIRE_THROWS_AS(MethodResultFromValue(input), InvocationError);
}


TEST_CASE("Method: parse results - any method", "[Method]") {
    const Value input = {
        eCommand::CALL,
        value_cast(Uid(0xFF)),
        value_cast(Uid(core::eMethod::Authenticate)),
        { value_cast(Uid(0)) },
        eCommand::END_OF_DATA,
        { 0, 0, 0 }
    };
    REQUIRE_THROWS_AS(MethodResultFromValue(input), std::invalid_argument);
}


TEST_CASE("Method: parse results - invalid input", "[Method]") {
    const Value input = {
        { 0 },
        eCommand::END_OF_DATA,
        { 0, 0 }  // Should have 3 numbers.
    };
    REQUIRE_THROWS_AS(MethodResultFromValue(input), std::invalid_argument);
}


constexpr Uid methodId = 0xFF;
constexpr Uid invokingId = 564;

using ReqInMethod = Method<methodId, 2, 0, 0, 0>;
using OptInMethod = Method<methodId, 0, 2, 0, 0>;
using ReqOutMethod = Method<methodId, 0, 0, 2, 0>;
using OptOutMethod = Method<methodId, 0, 0, 0, 2>;


TEST_CASE("Method: call method - required args only", "[Method]") {
    ReqInMethod method;
    CallContext context{
        invokingId,
        [](const MethodCall& call) -> asyncpp::task<MethodResult> {
            REQUIRE(call.args.size() == 2);
            REQUIRE(call.args[0] == 0);
            REQUIRE(call.args[1] == 1);
            co_return MethodResult{
                .values = {},
                .status = eMethodStatus::SUCCESS,
            };
        },
    };
    [[maybe_unused]] const std::tuple<> result = join(method(context, 0, 1));
}


TEST_CASE("Method: call method - optional args only", "[Method]") {
    OptInMethod method;
    SECTION("zero args") {
        CallContext context{
            invokingId,
            [](const MethodCall& call) -> asyncpp::task<MethodResult> {
                REQUIRE(call.args.size() == 0);
                co_return MethodResult{
                    .values = {},
                    .status = eMethodStatus::SUCCESS,
                };
            },
        };
        [[maybe_unused]] const std::tuple<> result = join(method(context, std::nullopt, std::nullopt));
    }
    SECTION("one arg") {
        CallContext context{
            invokingId,
            [](const MethodCall& call) -> asyncpp::task<MethodResult> {
                REQUIRE(call.args.size() == 1);
                REQUIRE(call.args[0].Is<Named>());
                REQUIRE(call.args[0].Get<Named>().name.Get<uint16_t>() == 0);
                REQUIRE(call.args[0].Get<Named>().value.Get<int>() == 10);
                co_return MethodResult{
                    .values = {},
                    .status = eMethodStatus::SUCCESS,
                };
            },
        };
        [[maybe_unused]] const std::tuple<> result = join(method(context, 10, std::nullopt));
    }
    SECTION("other arg") {
        CallContext context{
            invokingId,
            [](const MethodCall& call) -> asyncpp::task<MethodResult> {
                REQUIRE(call.args.size() == 1);
                REQUIRE(call.args[0].Is<Named>());
                REQUIRE(call.args[0].Get<Named>().name.Get<uint16_t>() == 1);
                REQUIRE(call.args[0].Get<Named>().value.Get<int>() == 11);
                co_return MethodResult{
                    .values = {},
                    .status = eMethodStatus::SUCCESS,
                };
            },
        };
        [[maybe_unused]] const std::tuple<> result = join(method(context, std::nullopt, 11));
    }
    SECTION("both args") {
        CallContext context{
            invokingId,
            [](const MethodCall& call) -> asyncpp::task<MethodResult> {
                REQUIRE(call.args.size() == 2);
                REQUIRE(call.args[0].Is<Named>());
                // Order maybe be important! (Check TCG core specification.)
                REQUIRE(call.args[0].Get<Named>().name.Get<uint16_t>() == 0);
                REQUIRE(call.args[0].Get<Named>().value.Get<int>() == 10);
                REQUIRE(call.args[1].Get<Named>().name.Get<uint16_t>() == 1);
                REQUIRE(call.args[1].Get<Named>().value.Get<int>() == 11);
                co_return MethodResult{
                    .values = {},
                    .status = eMethodStatus::SUCCESS,
                };
            },
        };
        [[maybe_unused]] const std::tuple<> result = join(method(context, 10, 11));
    }
}


TEST_CASE("Method: call method - required results only", "[Method]") {
    ReqOutMethod method;
    CallContext context{
        invokingId,
        [](const MethodCall& call) -> asyncpp::task<MethodResult> {
            REQUIRE(call.args.size() == 0);
            co_return MethodResult{
                .values = {0, 1},
                .status = eMethodStatus::SUCCESS,
            };
        },
    };
    const std::tuple<Value, Value> result = join(method(context));
    REQUIRE(std::get<0>(result).Get<int>() == 0);
    REQUIRE(std::get<1>(result).Get<int>() == 1);
}


TEST_CASE("Method: call method - optional results only", "[Method]") {
    OptOutMethod method;
    SECTION("zero results") {
        CallContext context{
            invokingId,
            [](const MethodCall& call) -> asyncpp::task<MethodResult> {
                REQUIRE(call.args.size() == 0);
                co_return MethodResult{
                    .values = {},
                    .status = eMethodStatus::SUCCESS,
                };
            },
        };
        const std::tuple<std::optional<Value>, std::optional<Value>> result = join(method(context));
        REQUIRE(std::get<0>(result) == std::nullopt);
        REQUIRE(std::get<1>(result) == std::nullopt);
    }
    SECTION("one result") {
        CallContext context{
            invokingId,
            [](const MethodCall& call) -> asyncpp::task<MethodResult> {
                REQUIRE(call.args.size() == 0);
                co_return MethodResult{
                    .values = { Named(0, 10) },
                    .status = eMethodStatus::SUCCESS,
                };
            },
        };
        const std::tuple<std::optional<Value>, std::optional<Value>> result = join(method(context));
        REQUIRE(std::get<0>(result) != std::nullopt);
        REQUIRE(std::get<0>(result)->Get<int>() == 10);
        REQUIRE(std::get<1>(result) == std::nullopt);
    }
    SECTION("other result") {
        CallContext context{
            invokingId,
            [](const MethodCall& call) -> asyncpp::task<MethodResult> {
                REQUIRE(call.args.size() == 0);
                co_return MethodResult{
                    .values = { Named(1, 11) },
                    .status = eMethodStatus::SUCCESS,
                };
            },
        };
        const std::tuple<std::optional<Value>, std::optional<Value>> result = join(method(context));
        REQUIRE(std::get<0>(result) == std::nullopt);
        REQUIRE(std::get<1>(result) != std::nullopt);
        REQUIRE(std::get<1>(result)->Get<int>() == 11);
    }
    SECTION("both results") {
        CallContext context{
            invokingId,
            [](const MethodCall& call) -> asyncpp::task<MethodResult> {
                REQUIRE(call.args.size() == 0);
                co_return MethodResult{
                    .values = {Named(0, 10), Named(1, 11)},
                    .status = eMethodStatus::SUCCESS,
                };
            },
        };
        const std::tuple<std::optional<Value>, std::optional<Value>> result = join(method(context));
        REQUIRE(std::get<0>(result) != std::nullopt);
        REQUIRE(std::get<0>(result)->Get<int>() == 10);
        REQUIRE(std::get<1>(result) != std::nullopt);
        REQUIRE(std::get<1>(result)->Get<int>() == 11);
    }
}