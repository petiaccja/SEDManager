#pragma once

#include "Structures/TokenStream.hpp"
#include "TrustedPeripheral.hpp"


class SessionManager {
public:
    SessionManager(std::shared_ptr<TrustedPeripheral> tper);
    SessionManager(const SessionManager&) = delete;
    SessionManager(SessionManager&&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;
    SessionManager& operator=(SessionManager&&) = delete;

    std::unordered_map<std::string, uint32_t> Properties(std::span<const Named<uint32_t>> hostProperties);

private:
    ComPacket Packetize(std::vector<uint8_t> payload);

private:
    static constexpr uint64_t INVOKING_ID = 0xFF;
    static constexpr uint8_t PROTOCOL = 0x01;
    std::shared_ptr<TrustedPeripheral> m_tper;
};