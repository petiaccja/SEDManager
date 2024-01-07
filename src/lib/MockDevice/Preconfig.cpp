#include "Preconfig.hpp"

#include <Messaging/Native.hpp>
#include <Specification/Common/Utility.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace sedmgr {

namespace mock {

    std::vector<std::shared_ptr<SecurityProvider>> GetMockPreconfig() {
        const auto adminSpUid = *Opal1Module::Get()->FindUid("SP::Admin");
        const auto lockingSpUid = *Opal1Module::Get()->FindUid("SP::Locking");

        const auto tables = {
            Table(
                core::eTable::Table,
                {
                    Object(UID(core::eTable::Table).ToDescriptor(), { value_cast("Table"sv), {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} }),
                    Object(UID(core::eTable::SP).ToDescriptor(), { value_cast("SP"sv), {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} }),
                }),
            Table(
                core::eTable::SP,
                {
                    Object(adminSpUid, { value_cast("Admin"sv), {}, {}, {}, {}, 0, false }),
                    Object(lockingSpUid, { value_cast("Locking"sv), {}, {}, {}, {}, 0, false }),
                }),
        };

        const auto adminSp = std::make_shared<SecurityProvider>(adminSpUid, tables);
        const auto lockingSp = std::make_shared<SecurityProvider>(lockingSpUid, std::initializer_list<Table>{});

        return { adminSp, lockingSp };
    }

} // namespace mock

} // namespace sedmgr