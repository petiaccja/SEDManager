#include "MethodUtils.hpp"

#include "Logging.hpp"

#include <Archive/Serialization.hpp>
#include <Error/Exception.hpp>
#include <Messaging/TokenStream.hpp>


namespace sedmgr {


ComPacket CreatePacket(std::vector<std::byte> payload, uint16_t comId, uint16_t comIdExt, uint32_t tperSessionNumber, uint32_t hostSessionNumber) {
    SubPacket subPacket;
    subPacket.kind = static_cast<uint16_t>(eSubPacketKind::DATA);
    subPacket.payload = std::move(payload);

    Packet packet;
    packet.tperSessionNumber = tperSessionNumber;
    packet.hostSessionNumber = hostSessionNumber;
    packet.sequenceNumber = 0;
    packet.ackType = 0;
    packet.acknowledgement = 0;
    packet.payload.push_back(std::move(subPacket));

    ComPacket comPacket;
    comPacket.comId = comId;
    comPacket.comIdExtension = comIdExt;
    comPacket.payload.push_back(std::move(packet));

    return comPacket;
}


std::span<const std::byte> UnwrapPacket(const ComPacket& packet) {
    const auto& p = !packet.payload.empty() ? packet.payload[0] : throw NoResponseError();
    const auto& s = !p.payload.empty() ? p.payload[0] : throw NoResponseError();
    return s.payload;
}


asyncpp::task<Value> SendPacketizedValue(std::shared_ptr<TrustedPeripheral> tper,
                                         uint8_t protocol,
                                         uint32_t tperSessionNumber,
                                         uint32_t hostSessionNumber,
                                         Value request,
                                         bool isRequestList) {
    const auto comId = tper->GetComId();
    const auto comIdExt = tper->GetComIdExtension();

    TokenStream requestStream{ Tokenize(request) };
    if (isRequestList) {
        requestStream = UnSurroundWithList(std::move(requestStream));
    }
    const auto requestBytes = Serialize(requestStream);
    const auto requestPacket = CreatePacket(requestBytes, comId, comIdExt, tperSessionNumber, hostSessionNumber);
    const auto responsePacket = co_await tper->SendPacket(protocol, requestPacket);
    const auto responseBytes = UnwrapPacket(responsePacket);
    auto responseStream = DeSerialize(Serialized<TokenStream>{ responseBytes });
    if (isRequestList) {
        responseStream = SurroundWithList(std::move(responseStream));
    }
    const Value response = DeTokenize(Tokenized<Value>{ responseStream.stream }).first;

    co_return response;
}


asyncpp::task<MethodResult> CallRemoteMethod(std::shared_ptr<TrustedPeripheral> tper,
                                             uint8_t protocol,
                                             uint32_t tperSessionNumber,
                                             uint32_t hostSessionNumber,
                                             MethodCall call) {
    const auto maybeMethodName = tper->GetModules().FindName(call.methodId);
    const auto methodName = maybeMethodName.value_or(call.methodId.ToString());

    const auto request = MethodCallToValue(call);
    Log(std::format("Host -> TPer >> '{}' [Session]", methodName), request);
    try {
        const Value response = co_await SendPacketizedValue(tper, protocol, tperSessionNumber, hostSessionNumber, request, true);
        Log(std::format("TPer -> Host << '{}' [Session]", methodName), response);

        MethodResult result = MethodResultFromValue(response);
        MethodStatusToException(methodName, result.status);
        co_return result;
    }
    catch (std::exception& ex) {
        Log(std::format("TPer -> Host << '{}' [Session] --- {}", methodName, ex.what()));
        throw;
    }
}


asyncpp::task<MethodResult> CallRemoteSessionMethod(std::shared_ptr<TrustedPeripheral> tper,
                                                    uint8_t protocol,
                                                    uint32_t tperSessionNumber,
                                                    uint32_t hostSessionNumber,
                                                    MethodCall call) {
    const auto maybeMethodName = tper->GetModules().FindName(call.methodId);
    const auto methodName = maybeMethodName.value_or(call.methodId.ToString());

    const auto request = MethodCallToValue(call);
    Log(std::format("Host -> TPer >> '{}' [SessionManager]", methodName), request);
    try {
        const Value response = co_await SendPacketizedValue(tper, protocol, tperSessionNumber, hostSessionNumber, request, true);
        Log(std::format("TPer -> Host << '{}' [SessionManager]", methodName), response);

        MethodCall result = MethodCallFromValue(response);
        MethodStatusToException(methodName, result.status);
        co_return { std::move(result.args), result.status };
    }
    catch (std::exception& ex) {
        Log(std::format("TPer -> Host << '{}' [SessionManager] --- {}", methodName, ex.what()));
        throw;
    }
}


} // namespace sedmgr