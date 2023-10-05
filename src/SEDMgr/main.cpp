#include <TPerLib/NvmeDevice.hpp>
#include <TPerLib/Serialization/TokenStreamArchive.hpp>
#include <TPerLib/Serialization/Utility.hpp>
#include <TPerLib/SessionManager.hpp>
#include <TPerLib/Structures/Packets.hpp>
#include <TPerLib/Structures/TokenStream.hpp>
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


template <class... Args>
std::vector<uint8_t> TokenizeManagementMethod(uint64_t methodId, const Args&... args) {
    constexpr uint64_t invokingId = 0xFF;
    std::stringstream buffer;
    TokenStreamOutputArchive ar(buffer);

    ar(Call{});
    ar(ToBytes(invokingId));
    ar(ToBytes(methodId));
    ar(List<const Args&...>{ args... });
    ar(EndOfData{});
    ar(List{ 0x00u, 0x00u, 0x00u });

    const auto ptr = reinterpret_cast<const uint8_t*>(buffer.view().data());
    return { ptr, ptr + buffer.view().size() };
}


std::vector<uint8_t> MethodBytes() {
    using namespace std::string_view_literals;

    std::stringstream buffer;
    TokenStreamOutputArchive tokenAr(buffer);

    uint64_t invokingId = 0xFF;
    uint64_t methodId = 0xFF01; // Properties

    tokenAr(Call{});
    tokenAr(ToBytes(invokingId));
    tokenAr(ToBytes(methodId));
    {
        tokenAr(StartList{});

        // Properties
        tokenAr(StartName{});
        tokenAr("HostProperties"sv);
        {
            tokenAr(StartList{});
            tokenAr(StartName{});
            tokenAr("MaxPackets"sv);
            tokenAr(1u);
            tokenAr(EndName{});
            tokenAr(EndList{});
        }
        tokenAr(EndName{});

        tokenAr(EndList{});
    }
    tokenAr(EndOfData{});
    {
        tokenAr(StartList{});
        tokenAr(0x00u);
        tokenAr(0x00u);
        tokenAr(0x00u);
        tokenAr(EndList{});
    }

    const auto ptr = reinterpret_cast<const uint8_t*>(buffer.view().data());
    return { ptr, ptr + buffer.view().size() };
}


void Properties(std::shared_ptr<TrustedPeripheral> tper) {
    auto sessionMgr = std::make_shared<SessionManager>(tper);

    const std::array hostProperties{ Named{ "MaxPackets", 1u } };
    const auto properties = sessionMgr->Properties(hostProperties);
    std::cout << "TPer properties:" << std::endl;
    for (auto& [name, value] : properties) {
        std::cout << "  " << name << " = " << value << std::endl;
    }

    SubPacket subPacket;
    subPacket.kind = static_cast<uint16_t>(eSubPacketKind::DATA);
    subPacket.payload = MethodBytes();

    Packet packet;
    packet.tperSessionNumber = 0;
    packet.hostSessionNumber = 0;
    packet.sequenceNumber = 0;
    packet.ackType = 0;
    packet.acknowledgement = 0;
    packet.payload.push_back(std::move(subPacket));

    ComPacket comPacket;
    comPacket.comId = tper->GetComId();
    comPacket.comIdExtension = tper->GetComIdExtension();
    comPacket.payload.push_back(std::move(packet));

    const auto request = ToBytes(comPacket);

    const auto result = tper->SendPacket(0x01, comPacket);

    std::cout << "Properties response:" << std::endl;
    std::cout << "  ComId:            " << result.comId << std::endl;
    std::cout << "  ComId extension:  " << result.comIdExtension << std::endl;
    std::cout << "  Min transfer:     " << result.minTransfer << std::endl;
    std::cout << "  Outstanding data: " << result.outstandingData << std::endl;
    std::cout << "  Payload:          " << result.payload.size() << " packets" << std::endl;
    std::cout << "ComID state: " << GetComIdStateStr(tper->VerifyComId()) << std::endl;
}


int main() {
    try {
        auto device = std::make_unique<NvmeDevice>("/dev/nvme0");
        const auto identity = device->IdentifyController();
        std::cout << ConvertRawCharacters(identity.modelNumber) << " "
                  << ConvertRawCharacters(identity.firmwareRevision) << std::endl;
        auto tper = std::make_shared<TrustedPeripheral>(std::move(device));

        PrintDeviceInfo(*tper);
        Properties(tper);
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}