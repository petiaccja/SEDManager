#include <async++/join.hpp>

#include <MockDevice/MockDevice.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>
#include <TrustedPeripheral/Session.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace sedmgr;
using namespace std::string_view_literals;


const auto adminSpUid = *Opal1Module::Get()->FindUid("SP::Admin");
const auto tableTableUid = *CoreModule::Get()->FindUid("Table");


struct SessionFixture {
    SessionFixture() {
        device = std::make_shared<MockDevice>();
        tper = std::make_shared<TrustedPeripheral>(device);
        sessionManager = std::make_shared<SessionManager>(tper);
        session = std::make_shared<Session>(sessionManager, adminSpUid);
    }
    std::shared_ptr<StorageDevice> device;
    std::shared_ptr<TrustedPeripheral> tper;
    std::shared_ptr<SessionManager> sessionManager;
    std::shared_ptr<Session> session;
};


TEST_CASE_METHOD(SessionFixture, "Session: Next", "Session") {
    auto first = join(session->base.Next(tableTableUid, {}));
    REQUIRE(first != Uid(0));
    auto second = join(session->base.Next(tableTableUid, first));
    REQUIRE(second != Uid(0));
}


TEST_CASE_METHOD(SessionFixture, "Session: Get", "Session") {
    REQUIRE(value_cast<Uid>(join(session->base.Get(adminSpUid, 0))) == adminSpUid);
    const auto all = join(session->base.Get(adminSpUid, 0, 8));
    REQUIRE(value_cast<std::string>(all[1]) == "Admin");
    REQUIRE(value_cast<int>(all[6]) == 0);
    REQUIRE(value_cast<bool>(all[7]) == false);
}


TEST_CASE_METHOD(SessionFixture, "Session: Set", "Session") {
    REQUIRE_NOTHROW(join(session->base.Set(adminSpUid, 2, value_cast("Stan"sv))));
    REQUIRE(value_cast<std::string_view>(join(session->base.Get(adminSpUid, 2))) == "Stan"sv);
}