#include "Preconfig.hpp"

#include <Messaging/Native.hpp>
#include <Specification/Common/Utility.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>
#include <Specification/PSID/PSIDModule.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace sedmgr {

namespace mock {

    std::shared_ptr<SecurityProvider> AdminPreconfig() {
        const auto adminSpUid = *Opal1Module::Get()->FindUid("SP::Admin");
        const auto lockingSpUid = *Opal1Module::Get()->FindUid("SP::Locking");

        const auto sidAuthUid = *CoreModule::Get()->FindUid("Authority::SID", adminSpUid);
        const auto psidAuthUid = *PSIDModule::Get()->FindUid("Authority::PSID", adminSpUid);
        const auto admin1AuthUid = *Opal1Module::Get()->FindUid("Authority::Admin1", adminSpUid);

        const auto tables = {
            Table(
                UID(core::eTable::Table),
                {
                    Object(UID(core::eTable::Table).ToDescriptor(), { value_cast("Table"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(UID(core::eTable::SP).ToDescriptor(), { value_cast("SP"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(UID(core::eTable::Authority).ToDescriptor(), { value_cast("Authority"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {} }),
                }),
            Table(
                UID(core::eTable::SP),
                {
                    Object(adminSpUid, { value_cast("Admin"sv), {}, {}, {}, {}, 0, false }),
                    Object(lockingSpUid, { value_cast("Locking"sv), {}, {}, {}, {}, 0, false }),
                }),
            Table(
                UID(core::eTable::Authority),
                {
                    Object(sidAuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(UID(0)), {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(psidAuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(UID(0)), {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(admin1AuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(UID(0)), {}, {}, {}, {}, {}, {}, {}, {} }),
                })
        };

        return std::make_shared<SecurityProvider>(adminSpUid, tables);
    }

    std::shared_ptr<SecurityProvider> LockingPreconfig() {
        const auto lockingSpUid = *Opal1Module::Get()->FindUid("SP::Locking");

        const auto admin1AuthUid = *Opal1Module::Get()->FindUid("Authority::Admin1", lockingSpUid);
        const auto user1AuthUid = *Opal1Module::Get()->FindUid("Authority::User1", lockingSpUid);

        const auto tables = {
            Table(
                UID(core::eTable::Table),
                {
                    Object(UID(core::eTable::Table).ToDescriptor(), { value_cast("Table"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(UID(core::eTable::Authority).ToDescriptor(), { value_cast("Authority"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {} }),
                }),
            Table(
                UID(core::eTable::Authority),
                {
                    Object(admin1AuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(UID(0)), {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(user1AuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(UID(0)), {}, {}, {}, {}, {}, {}, {}, {} }),
                })
        };

        return std::make_shared<SecurityProvider>(lockingSpUid, tables);
    }

    std::vector<std::shared_ptr<SecurityProvider>> GetMockPreconfig() {
        return { AdminPreconfig(), LockingPreconfig() };
    }

} // namespace mock

} // namespace sedmgr