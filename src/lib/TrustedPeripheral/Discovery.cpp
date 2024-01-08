#include "Discovery.hpp"

#include <Archive/Serialization.hpp>

#include <cereal/archives/portable_binary.hpp>

#include <format>


namespace sedmgr {

template <class... DescOptions>
void ParseDesc(std::span<const std::byte> featurePayloadBytes, uint16_t featureCode, std::optional<std::variant<DescOptions...>>& out) {
    const auto parsePayload = [&]<class Desc> {
        if (featureCode == Desc::featureCode) {
            auto desc = DeSerialize(Serialized<Desc>{ featurePayloadBytes });
            out = std::variant<DescOptions...>(std::move(desc));
        }
    };
    (..., parsePayload.template operator()<DescOptions>());
}


TPerDesc ParseTPerDesc(std::span<const std::byte> bytes) {
    constexpr int headerSizeBytes = 48;

    if (bytes.size_bytes() < headerSizeBytes) {
        throw std::invalid_argument(std::format("TPer desc must be at least {} bytes", headerSizeBytes));
    }

    const auto headerBytes = bytes.subspan(0, headerSizeBytes);
    const auto featureBytes = bytes.subspan(headerSizeBytes);

    const auto discoveryHeader = DeSerialize(Serialized<DiscoveryHeader>{ headerBytes });

    TPerDesc tperDesc;

    int64_t size = discoveryHeader.lengthOfData - sizeof(discoveryHeader.lengthOfData);
    int64_t offset = 0;

    while (offset < size) {
        const auto featureHeaderBytes = featureBytes.subspan(offset, 4);
        const auto featureHeader = DeSerialize(Serialized<FeatureDescHeader>{ featureHeaderBytes });
        const auto featurePayloadBytes = featureBytes.subspan(offset + 4, featureHeader.length);
        if (featureHeader.featureCode == TPerFeatureDesc::featureCode) {
            const auto desc = DeSerialize(Serialized<TPerFeatureDesc>{ featurePayloadBytes });
            tperDesc.tperDesc = desc;
        }
        if (featureHeader.featureCode == LockingFeatureDesc::featureCode) {
            const auto desc = DeSerialize(Serialized<LockingFeatureDesc>{ featurePayloadBytes });
            tperDesc.lockingDesc = desc;
        }
        else {
            std::optional<SSCFeatureDesc> sscFeatureDesc;
            ParseDesc(featurePayloadBytes, featureHeader.featureCode, sscFeatureDesc);
            if (sscFeatureDesc) {
                tperDesc.sscDescs.push_back(std::move(*sscFeatureDesc));
            }
        }

        offset += 4 + featureHeader.length;
    }
    return tperDesc;
}

} // namespace sedmgr