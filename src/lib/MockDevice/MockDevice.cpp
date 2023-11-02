#include "MockDevice.hpp"

#include <Archive/Conversion.hpp>
#include <Archive/Types/ValueToToken.hpp>
#include <Data/Value.hpp>
#include <Error/Exception.hpp>
#include <Specification/Core/CoreModule.hpp>

#include <atomic>


namespace sedmgr {


class MockSession {
public:
    MockSession(uint16_t comId, uint16_t comIdExt) : m_comId(comId), m_comIdExt(comIdExt) {}

    void Input(std::span<const std::byte> data);
    void Output(std::span<std::byte> data);

private:
    void SessionManagerInput(const Method& method);
    void SessionInput(Uid invokingId, const Method& method);

    void StartSession(const Method& method);
    void EndSession();
    void EnqueueResponse(const Value& value);

private:
    const uint16_t m_comId;
    const uint16_t m_comIdExt;
    std::optional<uint32_t> m_tsn;
    std::optional<uint32_t> m_hsn;
    std::queue<std::vector<std::byte>> m_responseQueue;
};


MockDevice::MockDevice() {
    AddOutputHandler(0x01, 0x0001, &MockDevice::Discovery);

    const auto baseSession = std::make_shared<MockSession>(baseComId, 0x0000);
    AddInputHandler(0x01, baseComId, [baseSession](std::span<const std::byte> data) { baseSession->Input(data); });
    AddOutputHandler(0x01, baseComId, [baseSession](std::span<std::byte> data) { baseSession->Output(data); });
}


StorageDeviceDesc MockDevice::GetDesc() {
    return StorageDeviceDesc{
        .name = "Mock Device",
        .serial = "MOCK0001",
        .interface = eStorageDeviceInterface::OTHER,
    };
}


void MockDevice::AddInputHandler(uint8_t protocol, uint16_t comId, InputHandler handler) {
    const auto [it, fresh] = m_inputHandlers.insert({
        {protocol, comId},
        std::move(handler)
    });
    assert(fresh); // Bug in MockDevice's code.
}


void MockDevice::AddOutputHandler(uint8_t protocol, uint16_t comId, OutputHandler handler) {
    const auto [it, fresh] = m_outputHandlers.insert({
        {protocol, comId},
        std::move(handler)
    });
    assert(fresh); // Bug in MockDevice's code.
}


void MockDevice::RemoveInputHandler(uint8_t protocol, uint16_t comId) {
    const auto it = m_inputHandlers.find({ protocol, comId });
    assert(it != m_inputHandlers.end()); // Bug in MockDevice's code.
    m_inputHandlers.erase(it);
}


void MockDevice::RemoveOutputHandler(uint8_t protocol, uint16_t comId) {
    const auto it = m_outputHandlers.find({ protocol, comId });
    assert(it != m_outputHandlers.end()); // Bug in MockDevice's code.
    m_outputHandlers.erase(it);
}


bool MockDevice::HasInputHandler(uint8_t protocol, uint16_t comId) const {
    return m_inputHandlers.contains({ protocol, comId });
}


bool MockDevice::HasOutputHandler(uint8_t protocol, uint16_t comId) const {
    return m_outputHandlers.contains({ protocol, comId });
}


void MockDevice::SecuritySend(uint8_t securityProtocol,
                              std::span<const std::byte, 2> protocolSpecific,
                              std::span<const std::byte> data) {
    const uint16_t comId = uint16_t(protocolSpecific[0]) | uint16_t(protocolSpecific[1]) << 8;
    const auto it = m_inputHandlers.find({ securityProtocol, comId });
    if (it == m_inputHandlers.end()) {
        throw DeviceError(std::format("IF_SEND: invalid security protocol ({}) / ComID ({})", securityProtocol, comId));
    }
    it->second(data);
}


void MockDevice::SecurityReceive(uint8_t securityProtocol,
                                 std::span<const std::byte, 2> protocolSpecific,
                                 std::span<std::byte> data) {
    const uint16_t comId = uint16_t(protocolSpecific[0]) | uint16_t(protocolSpecific[1]) << 8;
    const auto it = m_outputHandlers.find({ securityProtocol, comId });
    if (it == m_outputHandlers.end()) {
        throw DeviceError(std::format("IF_RECV: invalid security protocol ({}) / ComID ({})", securityProtocol, comId));
    }
    it->second(data);
}


void MockDevice::Discovery(std::span<std::byte> data) {
    constexpr std::array discovery = {
        // Discovery header
        0_b, 0_b, 0_b, 52_b, // Length of data
        0_b, 0_b, // Version major
        0_b, 0_b, // Version minor
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // VU
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // VU
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // VU
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // VU
        // TPer desc
        0x00_b, 0x01_b, // Feature code
        0x10_b, // Version
        0x0C_b, // Length
        0b0001'0001_b, // Bitmask
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, // Reserved
        // Locking desc
        0x00_b, 0x02_b, // Feature code
        0x10_b, // Version
        0x0C_b, // Length
        0b0000'0000_b, // Bitmask
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, // Reserved
        // Mock SSC desc == Opal v1 for now
        0x02_b, 0x00_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        std::byte(baseComId >> 8), std::byte(baseComId), // Base ComID
        0x00_b, 0x01_b, // Num ComIDs
        0x00_b, // Reserved
        0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, // Reserved
    };
    if (data.size() < discovery.size()) {
        throw DeviceError("receive buffer too small");
    }
    std::ranges::copy(discovery, data.begin());
}


void MockSession::Input(std::span<const std::byte> data) {
    ComPacket comPacket;
    FromBytes(data, comPacket);
    if (comPacket.comId != m_comId || comPacket.comIdExtension) {
        throw DeviceError("packet contains invalid ComID or ComIDExtension");
    }
    if (comPacket.payload.empty()) {
        return;
    }
    const auto& packet = comPacket.payload[0];
    if (packet.payload.empty()) {
        return;
    }
    const auto subPacket = packet.payload[0];
    if (subPacket.payload.size() != subPacket.PayloadLength()) {
        throw DeviceError("invalid SubPacket payload");
    }
    Value value;
    FromTokens(subPacket.payload, value);
    if (value.IsList()) {
        const auto& items = value.GetList();
        if (items.size() >= 1 && items[0].IsCommand() && items[0].GetCommand() == eCommand::END_OF_SESSION) {
            EndSession();
        }
        else {
            try {
                const auto method = MethodFromValue(value);
                uint64_t invokingId = 0;
                FromBytes(items[1].GetBytes(), invokingId);
                if (invokingId == 0xFF) {
                    SessionManagerInput(method);
                }
                else {
                    SessionInput(invokingId, method);
                }
            }
            catch (std::exception&) {
                throw DeviceError("invalid method call format");
            }
        }
    }
}


void MockSession::Output(std::span<std::byte> data) {
    static const ComPacket emptyPacket{
        .comId = m_comId,
        .comIdExtension = m_comIdExt,
        .outstandingData = 0,
        .minTransfer = 0,
    };
    static const auto emptyResponse = ToBytes(emptyPacket);


    if (m_responseQueue.empty()) {
        if (emptyResponse.size() <= data.size()) {
            std::ranges::copy(emptyResponse, data.begin());
        }
        else {
            throw DeviceError("receive buffer too small");
        }
    }
    else {
        const auto& response = m_responseQueue.front();
        if (response.size() <= data.size()) {
            std::ranges::copy(response, data.begin());
            m_responseQueue.pop();
        }
        else {
            const ComPacket outstandingPacket{
                .comId = m_comId,
                .comIdExtension = m_comIdExt,
                .outstandingData = uint32_t(response.size()),
                .minTransfer = uint32_t(response.size()),
            };
            const auto outstandingResponse = ToBytes(outstandingPacket);
            if (outstandingResponse.size() <= data.size()) {
                std::ranges::copy(outstandingResponse, data.begin());
            }
            else {
                throw DeviceError("receive buffer too small");
            }
        }
    }
}


void MockSession::SessionManagerInput(const Method& method) {
    switch (core::eMethod(method.methodId)) {
        case core::eMethod::StartSession: StartSession(method); break;
        default: break;
    }
}


void MockSession::SessionInput(Uid invokingId, const Method& method) {
}


void MockSession::StartSession(const Method& method) {
    static std::atomic_uint32_t nextTsn = 0x1000;

    // Handle if a session is already active
    if (m_tsn) {
        Method reply{ core::eMethod::SyncSession, {}, eMethodStatus::SP_BUSY };
        EnqueueResponse(MethodToValue(0xFF, reply));
        return;
    }

    // Parse arguments
    try {
        using Args = std::tuple<
            uint32_t, // hostSessionID
            Uid, // spId
            bool, // write
            std::optional<std::vector<std::byte>>, // hostChallenge
            std::optional<Uid>, // hostExchangeAuthority
            std::optional<std::vector<std::byte>>, // hostExchangeCert
            std::optional<Uid>, // hostSigningAuthority
            std::optional<std::vector<std::byte>>, // hostSigningCert
            std::optional<uint32_t>, // sessionTimeout
            std::optional<uint32_t>, // transTimeout
            std::optional<uint32_t>, // initialCredit
            std::optional<std::vector<std::byte>> // signedHash
            >;
        Args args;
        std::apply([&]<class... Args>(Args&&... args) { ArgsFromValues(method.args, std::forward<Args>(args)...); }, args);
        auto& [hsn, spId, write, hostChallenge, hostExchAuth, hostExchCert, hostSnAuth,
               hostSnCert, timeout, transTimeout, initialCredit, signedHash] = args;


        Method reply{ core::eMethod::SyncSession, ArgsToValues(hsn, nextTsn.fetch_add(1)), eMethodStatus::SUCCESS };
        EnqueueResponse(MethodToValue(0xFF, reply));
    }
    catch (std::exception&) {
        Method reply{ core::eMethod::SyncSession, {}, eMethodStatus::INVALID_PARAMETER };
        EnqueueResponse(MethodToValue(0xFF, reply));
    }
}


void MockSession::EndSession() {
    m_responseQueue = {};
    m_tsn = std::nullopt;
    m_hsn = std::nullopt;
    EnqueueResponse(Value(eCommand::END_OF_SESSION));
}


void MockSession::EnqueueResponse(const Value& value) {
    std::stringstream ss(std::ios::binary | std::ios::out);
    TokenBinaryOutputArchive ar(ss);
    save_strip_list(ar, value);
    const auto bytes = std::as_bytes(std::span(ss.view()));
    SubPacket subPacket{
        .kind = uint16_t(eSubPacketKind::DATA), .payload = {bytes.begin(), bytes.end()}
    };
    Packet packet{
        .tperSessionNumber = m_tsn.value_or(0),
        .hostSessionNumber = m_hsn.value_or(0),
        .payload = { std::move(subPacket) }
    };
    ComPacket comPacket{
        .comId = m_comId,
        .comIdExtension = m_comIdExt,
        .outstandingData = 0,
        .minTransfer = 0,
        .payload = { std::move(packet) },
    };
    m_responseQueue.push(ToBytes(comPacket));
}


} // namespace sedmgr