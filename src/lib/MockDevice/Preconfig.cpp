#include "Preconfig.hpp"

#include <Messaging/Native.hpp>
#include <Specification/Common/Utility.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>
#include <Specification/PSID/PSIDModule.hpp>
#include <TrustedPeripheral/ModuleCollection.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace sedmgr {

namespace mock {

    std::shared_ptr<SecurityProvider> AdminPreconfig() {
        ModuleCollection modules;
        modules.Load(Opal1Module::Get());

        const auto adminSpUid = modules.FindUid("SP::Admin").value();
        const auto lockingSpUid = modules.FindUid("SP::Locking").value();

        const auto sidAuthUid = modules.FindUid("Authority::SID", adminSpUid).value();
        const auto psidAuthUid = modules.FindUid("Authority::PSID", adminSpUid).value();
        const auto admin1AuthUid = modules.FindUid("Authority::Admin1", adminSpUid).value();

        const auto cPinMsidUid = modules.FindUid("C_PIN::MSID", adminSpUid).value();
        const auto cPinPsidUid = modules.FindUid("C_PIN::PSID", adminSpUid).value();
        const auto cPinSidUid = modules.FindUid("C_PIN::SID", adminSpUid).value();
        const auto cPinAdmin1Uid = modules.FindUid("C_PIN::Admin1", adminSpUid).value();

        const auto pinMsid = value_cast(std::as_bytes(std::span(std::string_view("1234"))));

        const auto tables = {
            Table(
                UID(core::eTable::Table),
                {
                    Object(UID(core::eTable::Table).ToDescriptor(), { value_cast("Table"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
                    Object(UID(core::eTable::SP).ToDescriptor(), { value_cast("SP"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
                    Object(UID(core::eTable::Authority).ToDescriptor(), { value_cast("Authority"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
                    Object(UID(core::eTable::C_PIN).ToDescriptor(), { value_cast("C_PIN"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
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
                    Object(sidAuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(cPinSidUid), {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(psidAuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(cPinPsidUid), {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(admin1AuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(cPinAdmin1Uid), {}, {}, {}, {}, {}, {}, {}, {} }),
                }),
            Table(
                UID(core::eTable::C_PIN),
                {
                    Object(cPinSidUid, { {}, {}, pinMsid, {}, {}, {}, {} }),
                    Object(cPinPsidUid, { {}, {}, pinMsid, {}, {}, {}, {} }),
                    Object(cPinMsidUid, { {}, {}, pinMsid, {}, {}, {}, {} }),
                    Object(cPinAdmin1Uid, { {}, {}, pinMsid, {}, {}, {}, {} }),
                })
        };

        return std::make_shared<SecurityProvider>(adminSpUid, tables);
    }

    std::shared_ptr<SecurityProvider> LockingPreconfig() {
        ModuleCollection modules;
        modules.Load(Opal1Module::Get());

        const auto lockingSpUid = modules.FindUid("SP::Locking").value();

        const auto admin1AuthUid = modules.FindUid("Authority::Admin1", lockingSpUid).value();
        const auto user1AuthUid = modules.FindUid("Authority::User1", lockingSpUid).value();

        const auto cPinAdmin1Uid = modules.FindUid("C_PIN::Admin1", lockingSpUid).value();
        const auto cPinUser1Uid = modules.FindUid("C_PIN::User1", lockingSpUid).value();

        const auto globalRangeUid = modules.FindUid("Locking::GlobalRange", lockingSpUid).value();
        const auto globalKeyUid = modules.FindUid("K_AES_256::GlobalRange", lockingSpUid).value();
        const auto globalKeyValue = Named(Bytes{ 0x00_b, 0x00_b, 0x02_b, 0x06_b }, value_cast("NSBD7IFTW5NW3BT583N5TBV35TV34C5N4V56B7534BV872NV325N6B34B6H4UIVB"sv));

        const auto lockingPin = value_cast(std::as_bytes(std::span(std::string_view("4567"))));

        const auto tables = {
            Table(
                UID(core::eTable::Table),
                {
                    Object(UID(core::eTable::Table).ToDescriptor(), { value_cast("Table"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
                    Object(UID(core::eTable::Authority).ToDescriptor(), { value_cast("Authority"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
                    Object(UID(core::eTable::C_PIN).ToDescriptor(), { value_cast("C_PIN"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
                    Object(UID(core::eTable::Locking).ToDescriptor(), { value_cast("Locking"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
                    Object(UID(core::eTable::K_AES_256).ToDescriptor(), { value_cast("K_AES_256"sv), {}, {}, 1, {}, {}, {}, {}, {}, {}, {}, {}, 0, 0 }),
                }),
            Table(
                UID(core::eTable::Authority),
                {
                    Object(admin1AuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(cPinAdmin1Uid), {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(user1AuthUid, { {}, {}, false, {}, true, {}, {}, {}, 1, value_cast(cPinUser1Uid), {}, {}, {}, {}, {}, {}, {}, {} }),
                }),
            Table(
                UID(core::eTable::C_PIN),
                {
                    Object(cPinAdmin1Uid, { {}, {}, lockingPin, {}, {}, {}, {} }),
                    Object(cPinUser1Uid, { {}, {}, lockingPin, {}, {}, {}, {} }),
                }),
            Table(UID(core::eTable::Locking),
                  {
                      Object(globalRangeUid, { value_cast("GlobalRange"sv), {}, 0, 16384, 0, 0, 0, 0, {}, value_cast(globalKeyUid), value_cast(UID(0)), {}, {}, {}, {}, {}, {}, {}, {} }),
                  }),
            Table(UID(core::eTable::K_AES_256),
                  {
                      Object(globalKeyUid, { value_cast("GlobalRange"sv), {}, globalKeyValue, {} }),
                  }),
        };

        return std::make_shared<SecurityProvider>(lockingSpUid, tables);
    }

    std::vector<std::shared_ptr<SecurityProvider>> GetMockPreconfig() {
        return { AdminPreconfig(), LockingPreconfig() };
    }

} // namespace mock

} // namespace sedmgr