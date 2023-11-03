#include "MockPreconfig.hpp"

#include <Specification/Common/Utility.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace sedmgr {

std::vector<MockSecurityProvider> GetMockPreconfig() {
    const auto adminSpUid = *Opal1Module::Get()->FindUid("SP::Admin");
    const auto lockingSpUid = *Opal1Module::Get()->FindUid("SP::Locking");

    const auto objects = {
        MockObject(TableToDescriptor(core::eTable::Table), { value_cast("Table"sv), {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} }),
        MockObject(TableToDescriptor(core::eTable::SP), { value_cast("SP"sv), {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} }),
        MockObject(adminSpUid, { value_cast("Admin"sv), {}, {}, {}, {}, 0, false }),
        MockObject(lockingSpUid, { value_cast("Locking"sv), {}, {}, {}, {}, 0, false }),
    };

    const auto tables = {
        MockTable(
            core::eTable::Table,
            {
                TableToDescriptor(core::eTable::Table),
                TableToDescriptor(core::eTable::SP),
            }),
        MockTable(
            core::eTable::SP,
            {
                adminSpUid,
                lockingSpUid,
            }),
    };

    MockSecurityProvider adminSp(adminSpUid, tables, objects);
    MockSecurityProvider lockingSp(lockingSpUid, {}, {});

    return { std::move(adminSp), std::move(lockingSp) };
}

} // namespace sedmgr