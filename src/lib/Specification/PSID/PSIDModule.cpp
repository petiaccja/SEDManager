#include "PSIDModule.hpp"

#include "../Common/Utility.hpp"

#include <unordered_map>



namespace {

constexpr std::initializer_list<std::pair<Uid, std::string_view>> names = {
    {0x0000'0009'0001'FF01,  "Authority::PSID"          },
    { 0x0000'000B'0001'FF01, "C_PIN::PSID"              },
    { 0x0000'0008'0001'00E1, "ACE::C_PIN_Get_PSID_NoPIN"},
    { 0x0000'0008'0001'00E0, "ACE::SP_PSID"             },
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


std::optional<std::string> PSIDModule::FindName(Uid uid, std::optional<Uid>) const {
    return GetFinder().Find(uid);
}


std::optional<Uid> PSIDModule::FindUid(std::string_view name, std::optional<Uid>) const {
    return GetFinder().Find(name);
}
