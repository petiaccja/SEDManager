#pragma once


#include "State.hpp"

#include <Messaging/ComPacket.hpp>
#include <Specification/Core/Defs/UIDs.hpp>
#include <Specification/Opal/OpalModule.hpp>
#include <StorageDevice/Common/StorageDevice.hpp>
#include <TrustedPeripheral/MethodUtils.hpp>

#include <map>
#include <span>
#include <vector>


namespace sedmgr {

namespace mock {

    class MessageHandler {
    public:
        virtual ~MessageHandler() = default;
        virtual bool SecuritySend(uint8_t securityProtocol,
                                  uint16_t comId,
                                  std::span<const std::byte> data) = 0;
        virtual bool SecurityReceive(uint8_t securityProtocol,
                                     uint16_t comId,
                                     std::span<std::byte> data) = 0;
    };

    class DiscoveryHandler : public MessageHandler {
    public:
        DiscoveryHandler(uint16_t baseComId);
        bool SecuritySend(uint8_t securityProtocol,
                          uint16_t comId,
                          std::span<const std::byte> data) override;
        bool SecurityReceive(uint8_t securityProtocol,
                             uint16_t comId,
                             std::span<std::byte> data) override;

        void Discovery(std::span<std::byte> data);

    private:
        uint16_t m_baseComId;
    };

    class ResetHandler : public MessageHandler {
    public:
        bool SecuritySend(uint8_t securityProtocol,
                          uint16_t comId,
                          std::span<const std::byte> data) override;
        bool SecurityReceive(uint8_t securityProtocol,
                             uint16_t comId,
                             std::span<std::byte> data) override;
    };

    class RequestComIdHandler : public MessageHandler {
    public:
        bool SecuritySend(uint8_t securityProtocol,
                          uint16_t comId,
                          std::span<const std::byte> data) override;
        bool SecurityReceive(uint8_t securityProtocol,
                             uint16_t comId,
                             std::span<std::byte> data) override;
    };

    class CommunicationLayerHandler : public MessageHandler {
    public:
        CommunicationLayerHandler(uint16_t comId, uint16_t comIdExt);
        bool SecuritySend(uint8_t securityProtocol,
                          uint16_t comId,
                          std::span<const std::byte> data) override;
        bool SecurityReceive(uint8_t securityProtocol,
                             uint16_t comId,
                             std::span<std::byte> data) override;

    private:
        void VerifyComIdValid();
        void StackReset();

    private:
        uint16_t m_comId;
        uint16_t m_comIdExt;
        std::optional<std::vector<std::byte>> m_response;
    };

    class SessionLayerHandler : public MessageHandler {
        struct SessionId {
            uint32_t tsn;
            uint32_t hsn;
            auto operator<=>(const SessionId&) const noexcept = default;
        };
        struct Session {
            std::shared_ptr<SecurityProvider> securityProvider;
        };

    public:
        SessionLayerHandler(uint16_t comId,
                            uint16_t comIdExt,
                            std::vector<std::shared_ptr<SecurityProvider>> securityProviders);
        bool SecuritySend(uint8_t securityProtocol,
                          uint16_t comId,
                          std::span<const std::byte> data) override;
        bool SecurityReceive(uint8_t securityProtocol,
                             uint16_t comId,
                             std::span<std::byte> data) override;

    private:
        void DecodeMethod(const Value& call, uint32_t tsn, uint32_t hsn);
        void DispatchMethod(const MethodCall&, uint32_t tsn, uint32_t hsn);
        void DispatchMethod(const MethodCall&);
        ComPacket Packetize(uint32_t tsn, uint32_t hsn, std::vector<std::byte> payload) const;

        template <class Executor, class Definition>
        MethodResult CallMethod(const MethodCall& query,
                                Session& session,
                                Executor&& executor,
                                Definition&& definition);

        template <class Executor, class Definition>
        MethodResult CallMethod(const MethodCall& query,
                                Executor&& executor,
                                Definition&& definition);

        void EndSession(uint32_t tperSessionNumber, uint32_t hostSessionNumber);

        auto Properties(std::optional<std::unordered_map<std::string, uint32_t>>) const
            -> std::pair<std::tuple<std::unordered_map<std::string, uint32_t>,
                                    std::optional<std::unordered_map<std::string, uint32_t>>>,
                         eMethodStatus>;

        auto StartSession(uint32_t hostSessionID,
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
                         eMethodStatus>;

        auto Get(Session& session, UID invokingId, CellBlock cellBlock) const
            -> std::pair<std::tuple<List>, eMethodStatus>;

        auto Set(Session& session, UID invokingId, std::optional<Value> where, std::optional<Value> values) const
            -> std::pair<std::tuple<>, eMethodStatus>;

        auto Next(Session& session, UID invokingId, std::optional<UID> where, std::optional<uint32_t> count) const
            -> std::pair<std::tuple<List>, eMethodStatus>;

        auto Authenticate(Session& session, UID invokingId, UID authority, std::optional<Bytes> proof) const
            -> std::pair<std::tuple<bool>, eMethodStatus>;

    private:
        static constexpr auto propertiesMethod = Method<UID(core::eMethod::Properties), 0, 1, 1, 1>{};
        static constexpr auto startSessionMethod = Method<UID(core::eMethod::StartSession), 3, 9, 2, 6>{};

        static constexpr auto getMethod = Method<UID(core::eMethod::Get), 1, 0, 1, 0>{};
        static constexpr auto setMethod = Method<UID(core::eMethod::Set), 0, 2, 0, 0>{};
        static constexpr auto nextMethod = Method<UID(core::eMethod::Next), 0, 2, 1, 0>{};
        static constexpr auto authenticateMethod = Method<UID(core::eMethod::Authenticate), 1, 1, 1, 0>{};
        static constexpr auto genKeyMethod = Method<UID(core::eMethod::GenKey), 0, 2, 0, 0>{};
        static constexpr auto revertMethod = Method<UID(opal::eMethod::Revert), 0, 0, 0, 0>{};
        static constexpr auto activateMethod = Method<UID(opal::eMethod::Activate), 0, 0, 0, 0>{};

        uint16_t m_comId;
        uint16_t m_comIdExt;
        std::vector<std::shared_ptr<SecurityProvider>> m_securityProviders;
        std::optional<std::vector<std::byte>> m_response;
        std::map<SessionId, Session> m_sessions;
        mutable uint32_t m_nextTsn = 5000;
    };

} // namespace mock


class MockDevice : public StorageDevice {
    struct SessionParams {
        uint32_t tsn;
        uint32_t hsn;
    };

public:
    MockDevice();

    StorageDeviceDesc GetDesc() override;
    void SecuritySend(uint8_t securityProtocol,
                      std::span<const std::byte, 2> protocolSpecific,
                      std::span<const std::byte> data) override;
    void SecurityReceive(uint8_t securityProtocol,
                         std::span<const std::byte, 2> protocolSpecific,
                         std::span<std::byte> data) override;

private:
    std::vector<std::shared_ptr<mock::SecurityProvider>> m_securityProviders;
    std::vector<std::unique_ptr<mock::MessageHandler>> m_messageHandlers;
    static constexpr uint16_t baseComId = 4097;
};

} // namespace sedmgr