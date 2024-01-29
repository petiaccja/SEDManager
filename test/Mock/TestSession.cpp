#include <async++/join.hpp>

#include <MockDevice/MockDevice.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>
#include <TrustedPeripheral/Session.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace sedmgr;
using namespace std::string_view_literals;


const auto adminSpUid = Opal1Module::Get() -> FindUid("SP::Admin").value();
const auto lockingSpUid = Opal1Module::Get() -> FindUid("SP::Locking").value();
const auto tableTableUid = CoreModule::Get() -> FindUid("Table").value();


struct SessionFixture {
    SessionFixture(UID securityProvider) {
        device = std::make_shared<MockDevice>();
        tper = std::make_shared<TrustedPeripheral>(device);
        sessionManager = std::make_shared<SessionManager>(tper);
        session = std::make_shared<Session>(sessionManager, securityProvider);
    }
    std::shared_ptr<StorageDevice> device;
    std::shared_ptr<TrustedPeripheral> tper;
    std::shared_ptr<SessionManager> sessionManager;
    std::shared_ptr<Session> session;
};

struct AdminSessionFixture : SessionFixture {
    AdminSessionFixture() : SessionFixture(adminSpUid) {}
};


struct LockingSessionFixture : SessionFixture {
    LockingSessionFixture() : SessionFixture(lockingSpUid) {}
};


TEST_CASE_METHOD(AdminSessionFixture, "Session: Next", "Session") {
    auto first = join(session->base.Next(tableTableUid, {}));
    REQUIRE(first != UID(0));
    auto second = join(session->base.Next(tableTableUid, first));
    REQUIRE(second != UID(0));
}


TEST_CASE_METHOD(AdminSessionFixture, "Session: Get", "Session") {
    REQUIRE(value_cast<UID>(join(session->base.Get(adminSpUid, 0))) == adminSpUid);
    const auto all = join(session->base.Get(adminSpUid, 0, 8));
    REQUIRE(value_cast<std::string>(all[1]) == "Admin");
    REQUIRE(value_cast<int>(all[6]) == 9);
    REQUIRE(value_cast<bool>(all[7]) == false);
}


TEST_CASE_METHOD(AdminSessionFixture, "Session: Set", "Session") {
    REQUIRE_NOTHROW(join(session->base.Set(adminSpUid, 2, value_cast("Stan"sv))));
    REQUIRE(value_cast<std::string_view>(join(session->base.Get(adminSpUid, 2))) == "Stan"sv);
}


TEST_CASE_METHOD(AdminSessionFixture, "Session: Authenticate", "Session") {
    const auto cPinMsid = Opal1Module::Get()->FindUid("C_PIN::MSID", adminSpUid).value();
    const auto sidAuthority = CoreModule::Get()->FindUid("Authority::SID", adminSpUid).value();
    SECTION("success") {
        const auto password = join(session->base.Get(cPinMsid, 3));
        REQUIRE_NOTHROW(join(session->base.Authenticate(sidAuthority, password.Get<Bytes>())));
    }
    SECTION("failure") {
        const auto password = Value(Bytes{ 0_b });
        REQUIRE_THROWS_AS(join(session->base.Authenticate(sidAuthority, password.Get<Bytes>())), PasswordError);
    }
}


TEST_CASE_METHOD(LockingSessionFixture, "Session: GenKey", "Session") {
    const auto globalKey = Opal1Module::Get()->FindUid("K_AES_256::GlobalRange", lockingSpUid).value();
    REQUIRE_NOTHROW(join(session->base.GenKey(globalKey)));
}


TEST_CASE_METHOD(AdminSessionFixture, "Session: Revert", "Session") {
    SECTION("Admin SP") {
        REQUIRE_NOTHROW(join(session->opal.Revert(adminSpUid)));
    }
    SECTION("Locking SP") {
        REQUIRE_NOTHROW(join(session->opal.Revert(lockingSpUid)));
    }
}

TEST_CASE_METHOD(AdminSessionFixture, "Session: Activate", "Session") {
    REQUIRE_NOTHROW(join(session->opal.Activate(lockingSpUid)));
}