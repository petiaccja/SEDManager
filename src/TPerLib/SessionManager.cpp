#include "SessionManager.hpp"

#include "Serialization/Utility.hpp"
#include "Structures/Packets.hpp"


SessionManager::SessionManager(std::shared_ptr<TrustedPeripheral> tper)
    : m_tper(tper){};


static std::vector<uint8_t> TokenizeMethodDynamic(uint64_t invokingId,
                                                  uint64_t methodId,
                                                  std::function<void(TokenStreamOutputArchive&)> args) {
    std::stringstream buffer;
    TokenStreamOutputArchive ar(buffer);

    ar(Call{});
    ar(ToBytes(invokingId));
    ar(ToBytes(methodId));
    ar(StartList{});
    args(ar);
    ar(EndList{});
    ar(EndOfData{});
    ar(List{ 0x00u, 0x00u, 0x00u });

    const auto ptr = reinterpret_cast<const uint8_t*>(buffer.view().data());
    return { ptr, ptr + buffer.view().size() };
}


template <class... Args>
static auto TokenizeMethod(uint64_t invokingId, uint64_t methodId, const Args&... args) {
    return TokenizeMethodDynamic(invokingId, methodId, [&args...](TokenStreamOutputArchive& ar) { (..., ar(args)); });
}


static std::tuple<uint64_t, uint64_t, unsigned> UntokenizeMethodDynamic(std::span<const uint8_t> bytes,
                                                                        std::function<void(TokenStreamInputArchive&)> args) {
    const auto ptr = reinterpret_cast<const char*>(bytes.data());
    std::stringstream buffer;
    buffer.str({ ptr, ptr + bytes.size() });
    TokenStreamInputArchive ar(buffer);

    std::array<uint8_t, 8> invokingIdBytes;
    std::array<uint8_t, 8> methodIdBytes;
    List<unsigned, unsigned, unsigned> statusCode;

    ar(Call{});
    ar(invokingIdBytes);
    ar(methodIdBytes);
    ar(StartList{});
    args(ar);
    ar(EndList{});
    ar(EndOfData{});
    ar(statusCode);

    uint64_t invokingId;
    uint64_t methodId;
    FromBytes(invokingIdBytes, invokingId);
    FromBytes(methodIdBytes, methodId);

    return { invokingId, methodId, std::get<0>(statusCode.values) };
}


template <class... Args>
static auto UntokenizeMethod(std::span<uint8_t> bytes, Args&... args) {
    return UntokenizeMethodDynamic([&args...](TokenStreamInputArchive& ar) {
        (..., ar(args));
    });
}


std::unordered_map<std::string, uint32_t> SessionManager::Properties(std::span<const Named<uint32_t>> hostProperties) {
    constexpr uint64_t METHOD_ID = 0xFF01;

    auto requestTokens = TokenizeMethodDynamic(INVOKING_ID, METHOD_ID, [hostProperties](TokenStreamOutputArchive& ar) {
        for (const auto& property : hostProperties) {
            ar(property);
        }
    });

    const auto request = Packetize(std::move(requestTokens));
    const auto response = m_tper->SendPacket(PROTOCOL, request);

    constexpr auto errorNoResponse = "no response to packet";
    const auto& packet = !response.payload.empty() ? response.payload[0] : throw std::runtime_error(errorNoResponse);
    const auto& sub = !packet.payload.empty() ? packet.payload[0] : throw std::runtime_error(errorNoResponse);
    const auto& responseTokens = sub.payload;

    std::unordered_map<std::string, uint32_t> properties;
    UntokenizeMethodDynamic(responseTokens, [&properties](TokenStreamInputArchive& ar) {
        while (ar.PeekTag() != EndList::Header()) {
            Named<uint32_t> property;
            ar(property);
            properties.insert_or_assign(property.name, property.value);
        }
    });
    return properties;
}


ComPacket SessionManager::Packetize(std::vector<uint8_t> payload) {
    SubPacket subPacket;
    subPacket.kind = static_cast<uint16_t>(eSubPacketKind::DATA);
    subPacket.payload = std::move(payload);

    Packet packet;
    packet.tperSessionNumber = 0;
    packet.hostSessionNumber = 0;
    packet.sequenceNumber = 0;
    packet.ackType = 0;
    packet.acknowledgement = 0;
    packet.payload.push_back(std::move(subPacket));

    ComPacket comPacket;
    comPacket.comId = m_tper->GetComId();
    comPacket.comIdExtension = m_tper->GetComIdExtension();
    comPacket.payload.push_back(std::move(packet));

    return comPacket;
}