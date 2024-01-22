#include "MockDevice.hpp"

#include "Preconfig.hpp"

#include <Archive/Serialization.hpp>
#include <Error/Exception.hpp>
#include <Messaging/TokenStream.hpp>
#include <Messaging/Value.hpp>
#include <Specification/Core/CoreModule.hpp>

#include <random>


namespace sedmgr {


MockDevice::MockDevice() {
    m_securityProviders = mock::GetMockPreconfig();

    m_messageHandlers.push_back(std::make_unique<mock::DiscoveryHandler>(baseComId));
    m_messageHandlers.push_back(std::make_unique<mock::ResetHandler>());
    m_messageHandlers.push_back(std::make_unique<mock::RequestComIdHandler>());
    m_messageHandlers.push_back(std::make_unique<mock::CommunicationLayerHandler>(baseComId, 0x0000));
    m_messageHandlers.push_back(std::make_unique<mock::SessionLayerHandler>(baseComId, 0x0000, m_securityProviders));
}


StorageDeviceDesc MockDevice::GetDesc() {
    return StorageDeviceDesc{
        .name = "Mock Device",
        .serial = "MOCK0001",
        .firmware = "MOCKFW01",
        .interface = eStorageDeviceInterface::OTHER,
    };
}


void MockDevice::SecuritySend(uint8_t securityProtocol,
                              std::span<const std::byte, 2> protocolSpecific,
                              std::span<const std::byte> data) {
    const uint16_t comId = uint16_t(protocolSpecific[0]) | uint16_t(protocolSpecific[1]) << 8;
    for (const auto& handler : m_messageHandlers) {
        if (handler->SecuritySend(securityProtocol, comId, data)) {
            return;
        }
    }
    throw DeviceError(std::format("IF_SEND: invalid security protocol ({}) / ComID ({})", securityProtocol, comId));
}


void MockDevice::SecurityReceive(uint8_t securityProtocol,
                                 std::span<const std::byte, 2> protocolSpecific,
                                 std::span<std::byte> data) {
    const uint16_t comId = uint16_t(protocolSpecific[0]) | uint16_t(protocolSpecific[1]) << 8;
    for (const auto& handler : m_messageHandlers) {
        if (handler->SecurityReceive(securityProtocol, comId, data)) {
            return;
        }
    }
    throw DeviceError(std::format("IF_RECV: invalid security protocol ({}) / ComID ({})", securityProtocol, comId));
}


namespace mock {

    //--------------------------------------------------------------------------
    // Discovery handler
    //--------------------------------------------------------------------------

    DiscoveryHandler::DiscoveryHandler(uint16_t baseComId) : m_baseComId(baseComId) {}


    bool DiscoveryHandler::SecuritySend(uint8_t securityProtocol,
                                        uint16_t comId,
                                        std::span<const std::byte> data) {
        return false;
    }


    bool DiscoveryHandler::SecurityReceive(uint8_t securityProtocol,
                                           uint16_t comId,
                                           std::span<std::byte> data) {
        if (securityProtocol == 0x01 && comId == 0x0001) {
            Discovery(data);
            return true;
        }
        return false;
    }


    void DiscoveryHandler::Discovery(std::span<std::byte> data) {
        const std::array discovery = {
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
            std::byte(m_baseComId >> 8), std::byte(m_baseComId), // Base ComID
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


    //--------------------------------------------------------------------------
    // Reset handler
    //--------------------------------------------------------------------------

    bool ResetHandler::SecuritySend(uint8_t securityProtocol,
                                    uint16_t comId,
                                    std::span<const std::byte> data) {
        if (securityProtocol == 0x02 && comId == 0x0004) {
            throw DeviceError("programmatic reset not implemented");
        }
        return false;
    }


    bool ResetHandler::SecurityReceive(uint8_t securityProtocol,
                                       uint16_t comId,
                                       std::span<std::byte> data) {
        return false;
    }


    //--------------------------------------------------------------------------
    // Request ComID handler
    //--------------------------------------------------------------------------

    bool RequestComIdHandler::SecuritySend(uint8_t securityProtocol,
                                           uint16_t comId,
                                           std::span<const std::byte> data) {
        return false;
    }


    bool RequestComIdHandler::SecurityReceive(uint8_t securityProtocol,
                                              uint16_t comId,
                                              std::span<std::byte> data) {
        if (securityProtocol == 0x02 && comId == 0x0000) {
            throw DeviceError("request ComID not implemented");
        }
        return false;
    }


    //--------------------------------------------------------------------------
    // Communication layer handler
    //--------------------------------------------------------------------------

    CommunicationLayerHandler::CommunicationLayerHandler(uint16_t comId, uint16_t comIdExt)
        : m_comId(comId), m_comIdExt(comIdExt) {}


    bool CommunicationLayerHandler::SecuritySend(uint8_t securityProtocol,
                                                 uint16_t comId,
                                                 std::span<const std::byte> data) {
        if (securityProtocol == 0x02 && comId == m_comId) {
            if (data.size() < 8) {
                throw DeviceError("Mock: IF-SEND 0x02: send buffer too small");
            }
            const auto comId = DeSerialize(Serialized<uint16_t>(data.subspan(0, 2)));
            const auto comIdExt = DeSerialize(Serialized<uint16_t>(data.subspan(2, 2)));
            const auto requestCode = DeSerialize(Serialized<uint32_t>(data.subspan(4, 4)));
            switch (requestCode) {
                case VerifyComIdValidRequest::requestCode: VerifyComIdValid(); break;
                case StackResetRequest::requestCode: StackReset(); break;
                default: throw DeviceError("Mock: IF-SEND 0x02: invalid request code");
            }
            return true;
        }
        return false;
    }


    bool CommunicationLayerHandler::SecurityReceive(uint8_t securityProtocol,
                                                    uint16_t comId,
                                                    std::span<std::byte> data) {
        if (securityProtocol == 0x02 && comId == m_comId) {
            if (!m_response) {
                throw DeviceError("Mock: IF-RECV 0x02: no response available");
            }
            if (m_response->size() > data.size()) {
                throw DeviceError("Mock: IF-RECV 0x02: receive buffer too small");
            }
            std::ranges::copy(*m_response, data.begin());
            m_response = std::nullopt;
            return true;
        }
        return false;
    }


    void CommunicationLayerHandler::VerifyComIdValid() {
        const VerifyComIdValidResponse response = {
            .comId = m_comId,
            .comIdExtension = m_comIdExt,
            .requestCode = VerifyComIdValidRequest::requestCode,
            .availableDataLength = 0x22,
            .comIdState = eComIdState::ISSUED,
            .timeOfAlloc = {},
            .timeOfExpiry = {},
            .timeCurrent = {},
        };
        m_response = Serialize(response);
    }


    void CommunicationLayerHandler::StackReset() {
        const StackResetResponse response = {
            .comId = m_comId,
            .comIdExtension = m_comIdExt,
            .requestCode = StackResetRequest::requestCode,
            .availableDataLength = 4,
            .success = eStackResetStatus::SUCCESS,
        };
        m_response = Serialize(response);
    }


    //--------------------------------------------------------------------------
    // Session layer handler
    //--------------------------------------------------------------------------

    SessionLayerHandler::SessionLayerHandler(uint16_t comId,
                                             uint16_t comIdExt,
                                             std::vector<std::shared_ptr<SecurityProvider>> securityProviders)
        : m_comId(comId),
          m_comIdExt(comIdExt),
          m_securityProviders(std::move(securityProviders)) {}


    bool SessionLayerHandler::SecuritySend(uint8_t securityProtocol,
                                           uint16_t comId,
                                           std::span<const std::byte> data) {
        if (securityProtocol == 0x01 && comId == m_comId) {
            const auto comPacket = DeSerialize(Serialized<ComPacket>{ data });
            if (comPacket.comId != m_comId || comPacket.comIdExtension) {
                throw DeviceError("packet contains invalid ComID or ComIDExtension");
            }
            if (comPacket.payload.empty()) {
                return true;
            }
            const auto& packet = comPacket.payload[0];
            if (packet.payload.empty()) {
                return true;
            }
            const auto subPacket = packet.payload[0];
            if (subPacket.payload.size() != subPacket.PayloadLength()) {
                throw DeviceError("invalid SubPacket payload");
            }
            const auto tokenStream = SurroundWithList(DeSerialize(Serialized<TokenStream>{ subPacket.payload }));
            const Value value = DeTokenize(Tokenized<Value>{ tokenStream.stream }).first;
            const auto tsn = packet.tperSessionNumber;
            const auto hsn = packet.hostSessionNumber;

            const auto& items = value.Get<List>();
            if (items.size() >= 1 && items[0].Is<eCommand>() && items[0].Get<eCommand>() == eCommand::END_OF_SESSION) {
                EndSession(tsn, hsn);
            }
            else {
                DecodeMethod(value, tsn, hsn);
            }
            return true;
        }
        return false;
    }


    bool SessionLayerHandler::SecurityReceive(uint8_t securityProtocol,
                                              uint16_t comId,
                                              std::span<std::byte> data) {
        if (securityProtocol == 0x01 && comId == m_comId) {
            static const ComPacket emptyPacket{
                .comId = m_comId,
                .comIdExtension = m_comIdExt,
                .outstandingData = 0,
                .minTransfer = 0,
            };
            static const auto emptyResponse = Serialize(emptyPacket);

            if (!m_response) {
                if (emptyResponse.size() <= data.size()) {
                    std::ranges::copy(emptyResponse, data.begin());
                }
                else {
                    throw DeviceError("receive buffer too small");
                }
            }
            else {
                if (m_response->size() <= data.size()) {
                    std::ranges::copy(*m_response, data.begin());
                    m_response = std::nullopt;
                }
                else {
                    const ComPacket outstandingPacket{
                        .comId = m_comId,
                        .comIdExtension = m_comIdExt,
                        .outstandingData = uint32_t(m_response->size()),
                        .minTransfer = uint32_t(m_response->size()),
                    };
                    const auto outstandingResponse = Serialize(outstandingPacket);
                    if (outstandingResponse.size() <= data.size()) {
                        std::ranges::copy(outstandingResponse, data.begin());
                    }
                    else {
                        throw DeviceError("receive buffer too small");
                    }
                }
            }
            return true;
        }
        return false;
    }


    void SessionLayerHandler::DecodeMethod(const Value& value, uint32_t tsn, uint32_t hsn) {
        try {
            const auto call = MethodCallFromValue(value);
            if (call.invokingId == UID(0xFF)) {
                DispatchMethod(call);
            }
            else {
                DispatchMethod(call, tsn, hsn);
            }
        }
        catch (std::invalid_argument&) {
            throw DeviceError("invalid method call format");
        }
    }

    void SessionLayerHandler::DispatchMethod(const MethodCall& call) {
        std::optional<MethodCall> reply;
        switch (core::eMethod(call.methodId)) {
            case core::eMethod::Properties: {
                const auto result = CallMethod(call, &SessionLayerHandler::Properties, propertiesMethod);
                reply = MethodCall(0xFF_uid, UID(core::eMethod::Properties), result.values, result.status);
                break;
            }
            case core::eMethod::StartSession: {
                const auto result = CallMethod(call, &SessionLayerHandler::StartSession, startSessionMethod);
                reply = MethodCall(0xFF_uid, UID(core::eMethod::SyncSession), result.values, result.status);
                break;
            }
            default: reply = std::nullopt; break;
        }
        if (!reply) {
            throw DeviceError(std::format("invalid/unsupported session layer method: {}", call.methodId.ToString()));
        }
        const auto tokenStream = UnSurroundWithList(TokenStream(Tokenize(MethodCallToValue(*reply))));
        auto response = Serialize(tokenStream);
        m_response = Serialize(Packetize(0, 0, std::move(response)));
    }

    void SessionLayerHandler::DispatchMethod(const MethodCall& call, uint32_t tsn, uint32_t hsn) {
        const auto sessionIt = m_sessions.find({ tsn, hsn });
        if (sessionIt == m_sessions.end()) {
            throw DeviceError("invalid session");
        }
        auto& session = sessionIt->second;
        const auto reply = [&]() -> std::optional<MethodResult> {
            switch (call.methodId.value) {
                case UID(core::eMethod::Get).value: return CallMethod(call, session, &SessionLayerHandler::Get, getMethod);
                case UID(core::eMethod::Set).value: return CallMethod(call, session, &SessionLayerHandler::Set, setMethod);
                case UID(core::eMethod::Next).value: return CallMethod(call, session, &SessionLayerHandler::Next, nextMethod);
                case UID(core::eMethod::Authenticate).value: return CallMethod(call, session, &SessionLayerHandler::Authenticate, authenticateMethod);
                case UID(core::eMethod::GenKey).value: return CallMethod(call, session, &SessionLayerHandler::GenKey, genKeyMethod);
                case UID(opal::eMethod::Revert).value: return CallMethod(call, session, &SessionLayerHandler::Revert, revertMethod);
                default: return std::nullopt;
            }
        }();
        if (!reply) {
            throw DeviceError(std::format("invalid/unsupported session layer method: {}", call.methodId.ToString()));
        }
        const auto tokenStream = UnSurroundWithList(TokenStream(Tokenize(MethodResultToValue(*reply))));
        auto response = Serialize(tokenStream);
        m_response = Serialize(Packetize(tsn, hsn, std::move(response)));
    }

    ComPacket SessionLayerHandler::Packetize(uint32_t tsn, uint32_t hsn, std::vector<std::byte> payload) const {
        SubPacket subPacket{
            .kind = static_cast<uint16_t>(eSubPacketKind::DATA),
            .payload = std::move(payload),
        };
        Packet packet{ tsn, hsn, 0, 0, 0, { std::move(subPacket) } };
        ComPacket comPacket{
            .comId = m_comId,
            .comIdExtension = m_comIdExt,
            .outstandingData = 0,
            .minTransfer = 0,
            .payload = { std::move(packet) }
        };
        return comPacket;
    }

    template <class Executor, class Definition>
    MethodResult SessionLayerHandler::CallMethod(const MethodCall& query,
                                                 Session& session,
                                                 Executor&& executor,
                                                 Definition&& definition) {
        const auto wrapper = [&](auto... inputs) {
            const auto [outputs, status] = (this->*executor)(session, query.invokingId, ConvertResult(inputs)...);
            return std::pair(std::apply([](auto... values) { return std::tuple(ConvertArg(values)...); }, outputs),
                             status);
        };
        auto [results, status] = definition.Execute(wrapper, query.args);
        return MethodResult{
            .values = std::move(results),
            .status = status,
        };
    }


    template <class Executor, class Definition>
    MethodResult SessionLayerHandler::CallMethod(const MethodCall& query,
                                                 Executor&& executor,
                                                 Definition&& definition) {
        const auto wrapper = [&](auto... inputs) {
            const auto [outputs, status] = (this->*executor)(ConvertResult(inputs)...);
            return std::pair(std::apply([](auto... values) { return std::tuple(ConvertArg(values)...); }, outputs),
                             status);
        };
        auto [results, status] = definition.Execute(wrapper, query.args);
        return MethodResult{
            .values = std::move(results),
            .status = status,
        };
    }


    void SessionLayerHandler::EndSession(uint32_t tperSessionNumber, uint32_t hostSessionNumber) {
        const auto sessionIt = m_sessions.find({ tperSessionNumber, hostSessionNumber });
        if (sessionIt != m_sessions.end()) {
            m_sessions.erase(sessionIt);
            const auto tokenStream = TokenStream(Tokenize(Value(eCommand::END_OF_SESSION)));
            auto response = Serialize(tokenStream);
            m_response = Serialize(Packetize(0, 0, std::move(response)));
        }
        else {
            throw DeviceError("invalid session");
        }
    }


    auto SessionLayerHandler::Properties(std::optional<std::unordered_map<std::string, uint32_t>>) const
        -> std::pair<std::tuple<std::unordered_map<std::string, uint32_t>,
                                std::optional<std::unordered_map<std::string, uint32_t>>>,
                     eMethodStatus> {
        const std::unordered_map<std::string, uint32_t> tperProperties = {
            {"MaxPackets",        1    },
            { "MaxSubpackets",    1    },
            { "MaxMethods",       1    },
            { "MaxComPacketSize", 65536},
            { "MaxIndTokenSize",  65536},
            { "MaxAggTokenSize",  65536},
            { "ContinuedTokens",  0    },
            { "SequenceNumbers",  0    },
            { "AckNAK",           0    },
            { "Asynchronous",     0    },
        };

        return {
            {tperProperties, std::nullopt},
            eMethodStatus::SUCCESS
        };
    }


    auto SessionLayerHandler::StartSession(uint32_t hostSessionID,
                                           UID spId,
                                           bool write,
                                           std::optional<std::vector<std::byte>> hostChallenge,
                                           std::optional<UID> hostExchangeAuthority,
                                           std::optional<std::vector<std::byte>> hostExchangeCert,
                                           std::optional<UID> hostSigningAuthority,
                                           std::optional<std::vector<std::byte>> hostSigningCert,
                                           std::optional<uint32_t> sessionTimeout,
                                           std::optional<uint32_t> transTimeout,
                                           std::optional<uint32_t> initialCredit,
                                           std::optional<std::vector<std::byte>> signedHash)
        -> std::pair<std::tuple<uint32_t,
                                uint32_t,
                                std::optional<Bytes>,
                                std::optional<Bytes>,
                                std::optional<Bytes>,
                                std::optional<uint32_t>,
                                std::optional<uint32_t>,
                                std::optional<Bytes>>,
                     eMethodStatus> {
        const auto spIt = std::ranges::find_if(m_securityProviders, [&](const auto& sp) { return sp->GetUID() == spId; });
        if (spIt == m_securityProviders.end()) {
            return {
                std::tuple(hostSessionID, 0, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
                eMethodStatus::INVALID_PARAMETER,
            };
        }

        const uint32_t tsn = m_nextTsn++;
        m_sessions.insert_or_assign(SessionId{ tsn, hostSessionID }, Session{ *spIt });
        return {
            std::tuple(hostSessionID, tsn, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            eMethodStatus::SUCCESS,
        };
    }


    auto SessionLayerHandler::Get(Session& session, UID invokingId, CellBlock cellBlock) const
        -> std::pair<std::tuple<List>, eMethodStatus> {
        if (invokingId.IsObject() || invokingId.IsDescriptor()) {
            const auto securityProvider = *session.securityProvider;
            const auto containingTableUid = invokingId.ContainingTable();
            if (!securityProvider.contains(containingTableUid)) {
                return { {}, eMethodStatus::INVALID_PARAMETER };
            }
            const auto& containingTable = securityProvider[containingTableUid];
            if (!containingTable.contains(invokingId)) {
                return { {}, eMethodStatus::INVALID_PARAMETER };
            }
            const auto& object = containingTable[invokingId];
            const auto firstColumn = cellBlock.startColumn.value_or(0);
            const auto lastColumn = cellBlock.endColumn.value_or(object.Size() - 1) + 1;
            if (firstColumn > lastColumn || lastColumn > object.Size()) {
                return { {}, eMethodStatus::INVALID_PARAMETER };
            }
            List values;
            for (auto i = firstColumn; i < lastColumn; ++i) {
                auto value = object[i];
                if (value.HasValue()) {
                    values.push_back(Named(i, object[i]));
                }
            }
            return { { std::move(values) }, eMethodStatus::SUCCESS };
        }
        else {
            throw NotImplementedError("byte tables are not implemented");
        }
    }


    auto SessionLayerHandler::Set(Session& session, UID invokingId, std::optional<Value> where, std::optional<Value> values) const
        -> std::pair<std::tuple<>, eMethodStatus> {
        if (invokingId.IsObject() || invokingId.IsDescriptor()) {
            auto& securityProvider = *session.securityProvider;
            const auto containingTableUid = invokingId.ContainingTable();
            if (!securityProvider.contains(containingTableUid)) {
                return { {}, eMethodStatus::INVALID_PARAMETER };
            }
            auto& containingTable = securityProvider[containingTableUid];
            if (!containingTable.contains(invokingId)) {
                return { {}, eMethodStatus::INVALID_PARAMETER };
            }
            auto& object = containingTable[invokingId];

            if (where) {
                return { {}, eMethodStatus::INVALID_PARAMETER };
            }
            if (!values || !values->Is<List>()) {
                return { {}, eMethodStatus::INVALID_PARAMETER };
            }

            const auto& list = values->Get<List>();

            for (const auto& item : list) {
                if (!item.Is<Named>()) {
                    return { {}, eMethodStatus::INVALID_PARAMETER };
                }
                const auto& [name, value] = item.Get<Named>();
                if (!name.IsInteger()) {
                    return { {}, eMethodStatus::INVALID_PARAMETER };
                }
                const auto column = name.Get<uint16_t>();
                if (column >= object.Size()) {
                    return { {}, eMethodStatus::INVALID_PARAMETER };
                }
            }
            for (const auto& item : list) {
                const auto& [name, value] = item.Get<Named>();
                object[name.Get<uint16_t>()] = value;
            }
            return { {}, eMethodStatus::SUCCESS };
        }
        else {
            throw NotImplementedError("byte tables are not implemented");
        }
    }


    auto SessionLayerHandler::Next(Session& session, UID invokingId, std::optional<UID> where, std::optional<uint32_t> count) const
        -> std::pair<std::tuple<List>, eMethodStatus> {
        auto& securityProvider = *session.securityProvider;
        if (!securityProvider.contains(invokingId)) {
            return { {}, eMethodStatus::INVALID_PARAMETER };
        }
        auto& table = securityProvider[invokingId];
        auto it = where ? ++table.find(*where) : table.begin();

        size_t numFound = 0;
        List next;
        while (it != table.end() && (!count || numFound < count.value())) {
            next.push_back(value_cast(it->second.GetUID()));
            ++it;
            ++numFound;
        }

        return { { next }, eMethodStatus::SUCCESS };
    }


    auto SessionLayerHandler::Authenticate(Session& session, UID invokingId, UID authority, std::optional<Bytes> proof) const
        -> std::pair<std::tuple<bool>, eMethodStatus> {
        const auto& securityProvider = *session.securityProvider;
        const auto& authorityTable = securityProvider[UID(core::eTable::Authority)];
        const auto& cPinTable = securityProvider[UID(core::eTable::C_PIN)];
        if (!authorityTable.contains(authority)) {
            return { { false }, eMethodStatus::INVALID_PARAMETER };
        }
        const auto& authorityObject = authorityTable[authority];
        const auto& credential = value_cast<UID>(authorityObject[10]);
        if (credential == UID(0)) {
            return { { true }, eMethodStatus::SUCCESS };
        }
        const auto& credentialObject = cPinTable[credential];
        const auto& pin = credentialObject[3];
        const auto success = pin.Get<Bytes>() == proof;
        return { { success }, eMethodStatus::SUCCESS };
    }


    auto SessionLayerHandler::GenKey(Session& session, UID invokingId, std::optional<uint32_t> publicExponent, std::optional<uint32_t> pinLength) const
        -> std::pair<std::tuple<>, eMethodStatus> {
        static constexpr auto characterSet = std::string_view("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
        thread_local std::mt19937_64 rne(std::chrono::high_resolution_clock::now().time_since_epoch().count());

        const auto randomKey = [](size_t length) {
            Bytes bytes;
            auto rng = std::uniform_int_distribution<size_t>(0, characterSet.size() - 1);
            for (int i = 0; i < length; ++i) {
                bytes.push_back(std::byte(characterSet[rng(rne)]));
            }
            return bytes;
        };

        auto& sp = *session.securityProvider;
        if (!invokingId.IsObject()) {
            return { {}, eMethodStatus::INVALID_PARAMETER };
        }
        const auto containingTableUid = invokingId.ContainingTable();
        if (!sp.contains(containingTableUid)) {
            return { {}, eMethodStatus::INVALID_PARAMETER };
        }
        auto& containingTable = sp[containingTableUid];
        if (!containingTable.contains(invokingId)) {
            return { {}, eMethodStatus::INVALID_PARAMETER };
        }
        auto& object = containingTable[invokingId];

        if (containingTableUid == UID(core::eTable::K_AES_256)) {
            object[3] = Named(Bytes{ 0x00_b, 0x00_b, 0x02_b, 0x06_b }, randomKey(64));
            return { {}, eMethodStatus::SUCCESS };
        }
        if (containingTableUid == UID(core::eTable::K_AES_128)) {
            object[3] = Named(Bytes{ 0x00_b, 0x00_b, 0x02_b, 0x05_b }, randomKey(32));
            return { {}, eMethodStatus::SUCCESS };
        }
        if (containingTableUid == UID(core::eTable::C_PIN)) {
            object[3] = Value(randomKey(pinLength.value_or(32)));
            return { {}, eMethodStatus::SUCCESS };
        }
        return { {}, eMethodStatus::INVALID_PARAMETER };
    }


    auto SessionLayerHandler::Revert(Session& session, UID invokingId) const
        -> std::pair<std::tuple<>, eMethodStatus> {
        auto& sp = *session.securityProvider;
        if (!sp.contains(UID(core::eTable::SP))) {
            return { {}, eMethodStatus::INVALID_PARAMETER };
        }
        const auto& spTable = sp[UID(core::eTable::SP)];
        if (!spTable.contains(invokingId)) {
            return { {}, eMethodStatus::INVALID_PARAMETER };
        }
        return { {}, eMethodStatus::SUCCESS };
    }

} // namespace mock

} // namespace sedmgr