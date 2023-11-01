#include "Discovery.hpp"

#include <Archive/Conversion.hpp>

#include <cereal/archives/portable_binary.hpp>

#include <format>


namespace sedmgr {

template <class... DescOptions>
void ParseDesc(std::span<const std::byte> featurePayloadBytes, uint16_t featureCode, std::optional<std::variant<DescOptions...>>& out) {
    const auto parsePayload = [&]<class Desc> {
        if (featureCode == Desc::featureCode) {
            Desc desc;
            FromBytes(featurePayloadBytes, desc);
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

    DiscoveryHeader discoveryHeader;
    FromBytes(headerBytes, discoveryHeader);

    TPerDesc tperDesc;

    int64_t size = discoveryHeader.lengthOfData - sizeof(discoveryHeader.lengthOfData);
    int64_t offset = 0;

    while (offset < size) {
        FeatureDescHeader featureHeader;
        const auto featureHeaderBytes = featureBytes.subspan(offset, 4);
        FromBytes(featureHeaderBytes, featureHeader);
        const auto featurePayloadBytes = featureBytes.subspan(offset + 4, featureHeader.length);
        if (featureHeader.featureCode == TPerFeatureDesc::featureCode) {
            TPerFeatureDesc desc;
            FromBytes(featurePayloadBytes, desc);
            tperDesc.tperDesc = desc;
        }
        if (featureHeader.featureCode == LockingFeatureDesc::featureCode) {
            LockingFeatureDesc desc;
            FromBytes(featurePayloadBytes, desc);
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