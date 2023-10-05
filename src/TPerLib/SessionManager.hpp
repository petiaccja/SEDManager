#pragma once

#include "Method.hpp"
#include "TrustedPeripheral.hpp"


class SessionManager {
public:
    SessionManager(std::shared_ptr<TrustedPeripheral> tper);
    SessionManager(const SessionManager&) = delete;
    SessionManager(SessionManager&&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;
    SessionManager& operator=(SessionManager&&) = delete;

    std::unordered_map<std::string, uint32_t> Properties(const std::optional<std::unordered_map<std::string, uint32_t>>& hostProperties = {});

private:
    ComPacket CreatePacket(std::vector<uint8_t> payload);
    std::span<const uint8_t> UnwrapPacket(const ComPacket& packet);
    Method InvokeMethod(const Method& method);

private:
    static constexpr uint64_t INVOKING_ID = 0xFF;
    static constexpr uint8_t PROTOCOL = 0x01;
    std::shared_ptr<TrustedPeripheral> m_tper;
};