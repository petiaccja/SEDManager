#include "MockSecurityProvider.hpp"

#include <Data/NativeTypes.hpp>
#include <TrustedPeripheral/Method.hpp>

#include <cstdint>
#include <queue>
#include <span>


namespace sedmgr {

class MockSession {
public:
    MockSession(std::shared_ptr<std::vector<MockSecurityProvider>> sps,
                uint16_t comId,
                uint16_t comIdExt);

    void Input(std::span<const std::byte> data);
    void Output(std::span<std::byte> data);

private:
    void SessionManagerInput(const MethodCall& method);
    void SessionInput(Uid invokingId, const MethodCall& method);

    void StartSession(const MethodCall& method);
    void Properties(const MethodCall& method);
    void EndSession();

    void Next(MockSecurityProvider& sp, Uid invokingId, const MethodCall& method);
    void Get(MockSecurityProvider& sp, Uid invokingId, const MethodCall& method);
    void Set(MockSecurityProvider& sp, Uid invokingId, const MethodCall& method);

    void EnqueueMethod(Uid invokingId, const MethodCall& method);
    void EnqueueMethodResult(const MethodResult& result);
    void EnqueueResponse(const Value& value);

private:
    const std::shared_ptr<std::vector<MockSecurityProvider>> m_sps;
    const uint16_t m_comId;
    const uint16_t m_comIdExt;
    std::optional<std::reference_wrapper<MockSecurityProvider>> m_sp;
    std::optional<uint32_t> m_tsn;
    std::optional<uint32_t> m_hsn;
    std::queue<std::vector<std::byte>> m_responseQueue;
};

} // namespace sedmgr