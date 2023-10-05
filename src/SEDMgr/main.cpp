#include <TPerLib/NvmeDevice.hpp>
#include <TPerLib/Serialization/Utility.hpp>
#include <TPerLib/SessionManager.hpp>
#include <TPerLib/Structures/Packets.hpp>
#include <TPerLib/TrustedPeripheral.hpp>

#include <iostream>
#include <ranges>
#include <string_view>
#include <unordered_map>


std::string_view ConvertRawCharacters(std::ranges::contiguous_range auto&& r) {
    return std::string_view(std::ranges::begin(r), std::ranges::end(r));
}


std::string_view GetSSCName(const StorageClassFeatureDesc& sscDesc) {
    if (std::holds_alternative<Opal2FeatureDesc>(sscDesc)) {
        return "Opal V2";
    }
    else if (std::holds_alternative<Pyrite1FeatureDesc>(sscDesc)) {
        return "Pyrite V1";
    }
    else if (std::holds_alternative<Pyrite2FeatureDesc>(sscDesc)) {
        return "Pyrite V2";
    }
    else if (std::holds_alternative<OpaliteFeatureDesc>(sscDesc)) {
        return "Opalite";
    }
    else {
        return "Unkown storage subsystem class";
    }
}


std::string_view GetComIdStateStr(eComIdState state) {
    switch (state) {
        case eComIdState::INVALID: return "invalid";
        case eComIdState::INACTIVE: return "inactive";
        case eComIdState::ISSUED: return "issued";
        case eComIdState::ASSOCIATED: return "associated";
    }
    return "unknown";
};


void PrintDeviceInfo(TrustedPeripheral& tper) {
    const auto& desc = tper.GetDesc();

    std::cout << std::format("  {}", GetSSCName(desc.sscDesc)) << std::endl;

    if (desc.tperDesc) {
        std::cout << "  TPer:" << std::endl;
        std::cout << std::format("    ComID management supported: {}", desc.tperDesc->comIdMgmtSupported) << std::endl;
    }
    if (desc.lockingDesc) {
        std::cout << "  Locking:" << std::endl;
        std::cout << std::format("    Locking supported:          {}", desc.lockingDesc->lockingSupported) << std::endl;
        std::cout << std::format("    Locking enabled:            {}", desc.lockingDesc->lockingEnabled) << std::endl;
        std::cout << std::format("    Locked:                     {}", desc.lockingDesc->locked) << std::endl;
        std::cout << std::format("    Media encryption supported: {}", desc.lockingDesc->mediaEncryption) << std::endl;
        std::cout << std::format("    Shadow MBR done:            {}", desc.lockingDesc->mbrDone) << std::endl;
        std::cout << std::format("    Shadow MBR enabled:         {}", desc.lockingDesc->mbrEnabled) << std::endl;
        std::cout << std::format("    Shadow MBR supported:       {}", desc.lockingDesc->mbrSupported) << std::endl;
    }
    if (!std::holds_alternative<std::monostate>(desc.sscDesc)) {
        std::cout << "  SSC:" << std::endl;
        const auto baseComId = std::visit([](const auto& arg) -> int {
            if constexpr (!std::is_convertible_v<decltype(arg), std::monostate>) {
                return arg.baseComId;
            }
            return 0;
        },
                                          desc.sscDesc);
        const auto numComIds = std::visit([](const auto& arg) -> int {
            if constexpr (!std::is_convertible_v<decltype(arg), std::monostate>) {
                return arg.numComIds;
            }
            return 0;
        },
                                          desc.sscDesc);
        std::cout << std::format("    Base ComID:                 {}", baseComId) << std::endl;
        std::cout << std::format("    Num ComIDs:                 {}", numComIds) << std::endl;
    }

    std::string_view comIdState = GetComIdStateStr(tper.VerifyComId());

    std::cout << "  Communication:" << std::endl;
    std::cout << std::format("    ComID:                      {}", tper.GetComId()) << std::endl;
    std::cout << std::format("    ComID extension:            {}", tper.GetComIdExtension()) << std::endl;
    std::cout << std::format("    ComID state:                {}", comIdState) << std::endl;
}


int main() {
    try {
        auto device = std::make_unique<NvmeDevice>("/dev/nvme0");
        const auto identity = device->IdentifyController();
        std::cout << ConvertRawCharacters(identity.modelNumber) << " "
                  << ConvertRawCharacters(identity.firmwareRevision) << std::endl;
        auto tper = std::make_shared<TrustedPeripheral>(std::move(device));

        PrintDeviceInfo(*tper);
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}