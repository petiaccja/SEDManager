#include <TPerLib/NvmeDevice.hpp>
#include <TPerLib/Serialization/Utility.hpp>
#include <TPerLib/Session.hpp>
#include <TPerLib/SessionManager.hpp>
#include <TPerLib/TrustedPeripheral.hpp>

#include <fstream>
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
        std::visit([](const auto& arg) {
            if constexpr (!std::is_convertible_v<decltype(arg), std::monostate>) {
                std::cout << std::format("    Base ComID:                 {}", arg.baseComId) << std::endl;
                std::cout << std::format("    Num ComIDs:                 {}", arg.numComIds) << std::endl;
                std::cout << std::format("    Initial C_PIN:              {}", arg.initialCPinSidIndicator) << std::endl;
                std::cout << std::format("    C_PIN revert behavior:      {}", arg.cPinSidRevertBehavior) << std::endl;
            }
        },
                   desc.sscDesc);
    }

    std::string_view comIdState = GetComIdStateStr(tper.VerifyComId());

    std::cout << "  Communication:" << std::endl;
    std::cout << std::format("    ComID:                      {}", tper.GetComId()) << std::endl;
    std::cout << std::format("    ComID extension:            {}", tper.GetComIdExtension()) << std::endl;
    std::cout << std::format("    ComID state:                {}", comIdState) << std::endl;
    std::cout << std::endl;
}


void PrintDeviceProperties(std::shared_ptr<SessionManager> sessionManager) {
    const std::unordered_map<std::string, uint32_t> hostProperties = {
        { "MaxPackets", 1 },
        { "MaxSubpackets", 1 },
        { "MaxMethods", 1 },
        { "MaxComPacketSize", 65536 },
        { "MaxIndTokenSize", 65536 },
        { "MaxAggTokenSize", 65536 },
        { "ContinuedTokens", 0 },
        { "SequenceNumbers", 0 },
        { "AckNAK", 0 },
        { "Asynchronous", 0 },
    };

    try {
        const auto& [tperProps, hostProps] = sessionManager->Properties(hostProperties);

        std::cout << "TPer properties: " << std::endl;
        for (const auto& [name, value] : tperProps) {
            std::cout << std::format("  {} = {}", name, value) << std::endl;
        }
        std::cout << std::endl;
    }
    catch (...) {
        std::cout << "Failed to get TPer properties.\n"
                  << std::endl;
    }
}

void DoRevert(std::shared_ptr<SessionManager> sessionManager) {
    std::ifstream file("/home/petiaccja/Programming/ssd_psid.txt");
    std::string password = { std::istreambuf_iterator(file.rdbuf()), std::istreambuf_iterator<char>() };
    try {
        std::cout << std::format("Starting session as PSID...\n");
        Session session(sessionManager, opal::eSecurityProvider::Admin, std::as_bytes(std::span(password)), opal::eAuthorityId::PSID);
        std::cout << std::format("  HSN = {}", session.GetHostSessionNumber()) << std::endl;
        std::cout << std::format("  TSN = {}", session.GetTPerSessionNumber()) << std::endl;

        std::cout << "Reverting..." << std::endl;
        session.opal.Revert(opal::eSecurityProvider::Admin);
    }
    catch (std::exception& ex) {
        std::cout << "Session failed: " << ex.what() << std::endl;
    }
}


void DoSession(std::shared_ptr<SessionManager> sessionManager) {
    std::vector<std::byte> password;
    try {
        std::cout << std::format("Starting session as Anybody...\n");
        Session session(sessionManager, opal::eSecurityProvider::Admin);
        std::cout << std::format("  HSN = {}", session.GetHostSessionNumber()) << std::endl;
        std::cout << std::format("  TSN = {}", session.GetTPerSessionNumber()) << std::endl;

        const auto msidPin = session.base.Get(opal::eTableRow_C_PIN::C_PIN_MSID, 3);
        const auto passwordView = msidPin.Get<std::span<const std::byte>>();
        password = { passwordView.begin(), passwordView.end() };
        std::cout << "MSID PIN: " << StringView(passwordView) << std::endl;
    }
    catch (std::exception& ex) {
        std::cout << "Session failed: " << ex.what() << std::endl;
    }

    try {
        std::cout << std::format("Starting session as SID...\n");
        Session session(sessionManager, opal::eSecurityProvider::Admin, password, eAuthorityId::SID);
        std::cout << std::format("  HSN = {}", session.GetHostSessionNumber()) << std::endl;
        std::cout << std::format("  TSN = {}", session.GetTPerSessionNumber()) << std::endl;
    }
    catch (std::exception& ex) {
        std::cout << "Session failed: " << ex.what() << std::endl;
    }
}


int main() {
    try {
        auto device = std::make_unique<NvmeDevice>("/dev/nvme1");
        const auto identity = device->IdentifyController();
        std::cout << ConvertRawCharacters(identity.modelNumber) << " "
                  << ConvertRawCharacters(identity.firmwareRevision) << std::endl;
        auto tper = std::make_shared<TrustedPeripheral>(std::move(device));
        auto sessionManager = std::make_shared<SessionManager>(tper);


        PrintDeviceInfo(*tper);
        PrintDeviceProperties(sessionManager);
        //DoRevert(sessionManager);
        DoSession(sessionManager);
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}