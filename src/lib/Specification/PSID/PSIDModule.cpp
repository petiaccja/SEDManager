#include "PSIDModule.hpp"

#include "../Common/Utility.hpp"

#include <unordered_map>


namespace sedmgr {

namespace {

    constexpr std::initializer_list<std::pair<UID, std::string_view>> names = {
        {0x0000'0009'0001'FF01_uid,  "Authority::PSID"          },
        { 0x0000'000B'0001'FF01_uid, "C_PIN::PSID"              },
        { 0x0000'0008'0001'00E1_uid, "ACE::C_PIN_Get_PSID_NoPIN"},
        { 0x0000'0008'0001'00E0_uid, "ACE::SP_PSID"             },
    };

    const NameAndUidFinder& GetFinder() {
        static NameAndUidFinder finder({ names }, {});
        return finder;
    }

} // namespace


std::shared_ptr<Module> PSIDModule::Get() {
    static const auto instance = std::make_shared<PSIDModule>();
    return instance;
}


std::string_view PSIDModule::ModuleName() const {
    return "PSID";
}


eModuleKind PSIDModule::ModuleKind() const {
    return eModuleKind::FEATURE;
}


std::optional<std::string> PSIDModule::FindName(UID uid, std::optional<UID>) const {
    return GetFinder().Find(uid);
}


std::optional<UID> PSIDModule::FindUid(std::string_view name, std::optional<UID>) const {
    return GetFinder().Find(name);
}

} // namespace sedmgr