#include "MockSession.hpp"

#include <Archive/Conversion.hpp>
#include <Data/ComPacket.hpp>
#include <Data/TokenStream.hpp>
#include <Data/Value.hpp>
#include <Error/Exception.hpp>
#include <Specification/Core/CoreModule.hpp>

#include <atomic>


namespace sedmgr {

MockSession::MockSession(std::shared_ptr<std::vector<MockSecurityProvider>> sps,
                         uint16_t comId,
                         uint16_t comIdExt) : m_sps(sps), m_comId(comId), m_comIdExt(comIdExt) {
    assert(m_sps);
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
    const auto tokenStream = SurroundWithList(DeSerialize(Serialized<TokenStream>{ subPacket.payload }));
    const Value value = DeTokenize(Tokenized<Value>{ tokenStream.stream }).first;
    if (value.Is<List>()) {
        const auto& items = value.Get<List>();
        if (items.size() >= 1 && items[0].Is<eCommand>() && items[0].Get<eCommand>() == eCommand::END_OF_SESSION) {
            EndSession();
        }
        else {
            try {
                const auto method = MethodCallFromValue(value);
                if (method.invokingId == Uid(0xFF)) {
                    SessionManagerInput(method);
                }
                else {
                    SessionInput(method);
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


void MockSession::SessionManagerInput(const MethodCall& method) {
    switch (core::eMethod(method.methodId)) {
        case core::eMethod::StartSession: StartSession(method); break;
        case core::eMethod::Properties: Properties(method); break;
        default: break;
    }
}


void MockSession::SessionInput(const MethodCall& method) {
    if (m_sp) {
        switch (core::eMethod(method.methodId)) {
            case core::eMethod::Next: Next(m_sp->get(), method); break;
            case core::eMethod::Get: Get(m_sp->get(), method); break;
            case core::eMethod::Set: Set(m_sp->get(), method); break;
            default: break;
        }
    }
}


void MockSession::StartSession(const MethodCall& method) {
    static std::atomic_uint32_t nextTsn = 0x1000;

    // Handle if a session is already active
    if (m_tsn) {
        MethodCall reply{ 0xFF, core::eMethod::SyncSession, {}, eMethodStatus::SP_BUSY };
        EnqueueMethod(reply);
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
        const auto& [hsn, spId, write, hostChallenge, hostExchAuth, hostExchCert, hostSnAuth,
                     hostSnCert, timeout, transTimeout, initialCredit, signedHash] = args;


        const auto spIt = std::ranges::find_if(*m_sps, [&](auto& sp) { return sp.GetUID() == spId; });
        if (spIt == m_sps->end()) {
            MethodCall reply{ 0xFF, core::eMethod::SyncSession, {}, eMethodStatus::INVALID_PARAMETER };
            EnqueueMethod(reply);
        }

        m_sp = *spIt;
        m_tsn = nextTsn.fetch_add(1);
        m_hsn = hsn;

        MethodCall reply{ 0xFF, core::eMethod::SyncSession, ArgsToValues(*m_hsn, *m_tsn), eMethodStatus::SUCCESS };
        EnqueueMethod(reply);
    }
    catch (std::exception&) {
        MethodCall reply{ 0xFF, core::eMethod::SyncSession, {}, eMethodStatus::INVALID_PARAMETER };
        EnqueueMethod(reply);
    }
}


void MockSession::Properties(const MethodCall& method) {
    using PropertyMap = std::unordered_map<std::string, uint32_t>;

    using Args = std::tuple<std::optional<PropertyMap>>;
    Args args;
    std::apply([&]<class... Args>(Args&&... args) { ArgsFromValues(method.args, std::forward<Args>(args)...); }, args);
    auto& [hostProperties] = args;

    const PropertyMap tperProperties = {
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

    MethodCall reply{ 0xFF, core::eMethod::SyncSession, ArgsToValues(tperProperties, hostProperties), eMethodStatus::SUCCESS };
    EnqueueMethod(reply);
}


void MockSession::EndSession() {
    m_responseQueue = {};
    m_sp = std::nullopt;
    m_tsn = std::nullopt;
    m_hsn = std::nullopt;
    EnqueueResponse(Value(List{ eCommand::END_OF_SESSION }));
}


void MockSession::Next(MockSecurityProvider& sp, const MethodCall& method) {
    using Args = std::tuple<std::optional<Uid>, std::optional<uint32_t>>;

    const auto maybeTable = sp.GetTable(method.invokingId);
    try {
        Args args;
        std::apply([&]<class... Args>(Args&&... args) { ArgsFromValues(method.args, std::forward<Args>(args)...); }, args);
        const auto& [from, count] = args;

        const auto& table = std::get<0>(maybeTable).get();
        std::vector<Uid> nexts;
        for (size_t i = 0; i < count.value_or(1); ++i) {
            const auto maybeNext = table.Next(nexts.empty() ? from.value_or(0) : nexts.back());
            try {
                const auto next = std::get<0>(maybeNext);
                if (next != Uid(0)) {
                    nexts.push_back(next);
                }
                else {
                    break;
                }
            }
            catch (std::exception&) {
                EnqueueMethodResult({ .values = {}, .status = eMethodStatus::INVALID_PARAMETER });
            }
        }
        std::vector<Value> results;
        std::ranges::transform(nexts, std::back_inserter(results), [](auto v) { return value_cast(v); });
        EnqueueMethodResult({ .values = { value_cast(results) }, .status = eMethodStatus::SUCCESS });
    }
    catch (std::exception&) {
        EnqueueMethodResult({ .values = {}, .status = eMethodStatus::INVALID_PARAMETER });
    }
}


void MockSession::Get(MockSecurityProvider& sp, const MethodCall& method) {
    using Args = std::tuple<CellBlock>;

    try {
        Args args;
        std::apply([&]<class... Args>(Args&&... args) { ArgsFromValues(method.args, std::forward<Args>(args)...); }, args);
        const auto& [cellBlock] = args;

        const auto maybeObject = sp.GetObject(method.invokingId);
        auto& object = std::get<0>(maybeObject).get();

        const auto startColumn = cellBlock.startColumn.value_or(0);
        const auto endColumn = cellBlock.endColumn.value_or(ptrdiff_t(object.Size()) - 1);

        auto maybeResult = object.Get(startColumn, endColumn);
        try {
            const auto result = std::get<0>(maybeResult);
            EnqueueMethodResult({ .values = { value_cast(result) }, .status = eMethodStatus::SUCCESS });
        }
        catch (std::exception&) {
            EnqueueMethodResult({ .values = {}, .status = std::get<1>(maybeResult) });
        }
    }
    catch (std::exception&) {
        EnqueueMethodResult({ .values = {}, .status = eMethodStatus::INVALID_PARAMETER });
    }
}


void MockSession::Set(MockSecurityProvider& sp, const MethodCall& method) {
    using Args = std::tuple<std::optional<Value>, std::optional<Value>>;

    try {
        Args args;
        std::apply([&]<class... Args>(Args&&... args) { ArgsFromValues(method.args, std::forward<Args>(args)...); }, args);
        const auto& [where, values] = args;

        const auto maybeObject = sp.GetObject(method.invokingId);
        auto& object = std::get<0>(maybeObject).get();

        if (values) {
            if (values->Is<List>()) {
                const auto mapping = value_cast<std::unordered_map<uint32_t, Value>>(*values);
                const auto result = object.Set(mapping);
                if (result.index() == 1) {
                    EnqueueMethodResult({ .values = {}, .status = std::get<1>(result) });
                    return;
                }
                else {
                    EnqueueMethodResult({ .values = {}, .status = eMethodStatus::SUCCESS });
                    return;
                }
            }
            else if (values->Is<Bytes>()) {
                throw NotImplementedError();
            }
        }
        EnqueueMethodResult({ .values = {}, .status = eMethodStatus::SUCCESS });
    }
    catch (std::exception&) {
        EnqueueMethodResult({ .values = {}, .status = eMethodStatus::INVALID_PARAMETER });
    }
}


void MockSession::EnqueueMethod(const MethodCall& method) {
    return EnqueueResponse(MethodCallToValue(method));
}


void MockSession::EnqueueMethodResult(const MethodResult& result) {
    EnqueueResponse(Value{
        result.values,
        eCommand::END_OF_DATA,
        {uint32_t(result.status), 0u, 0u}
    });
}


void MockSession::EnqueueResponse(const Value& value) {
    const auto tokenStream = UnSurroundWithList(TokenStream{ Tokenize(value) });
    const auto bytes = Serialize(tokenStream);
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