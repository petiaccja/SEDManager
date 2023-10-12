#include "Discovery.hpp"

#include "../Serialization/Utility.hpp"

#include <cereal/archives/portable_binary.hpp>

#include <format>


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
        FromBytes(featureBytes.subspan(offset, 4), featureHeader);
        offset += 4;
        switch (static_cast<eDiscoveryFeatureCode>(featureHeader.featureCode)) {
            case eDiscoveryFeatureCode::TPER: {
                TPerFeatureDesc desc;
                FromBytes(featureBytes.subspan(offset, featureHeader.length), desc);
                tperDesc.tperDesc = desc;
                break;
            }
            case eDiscoveryFeatureCode::LOCKING: {
                LockingFeatureDesc desc;
                FromBytes(featureBytes.subspan(offset, featureHeader.length), desc);
                tperDesc.lockingDesc = desc;
                break;
            }
            case eDiscoveryFeatureCode::OPAL2: {
                Opal2FeatureDesc desc;
                FromBytes(featureBytes.subspan(offset, featureHeader.length), desc);
                tperDesc.sscDesc = desc;
                break;
            }
            case eDiscoveryFeatureCode::OPALITE: {
                OpaliteFeatureDesc desc;
                FromBytes(featureBytes.subspan(offset, featureHeader.length), desc);
                tperDesc.sscDesc = desc;
                break;
            }
            case eDiscoveryFeatureCode::PYRITE1: {
                Pyrite1FeatureDesc desc;
                FromBytes(featureBytes.subspan(offset, featureHeader.length), desc);
                tperDesc.sscDesc = desc;
                break;
            }
            case eDiscoveryFeatureCode::PYRITE2: {
                Pyrite2FeatureDesc desc;
                FromBytes(featureBytes.subspan(offset, featureHeader.length), desc);
                tperDesc.sscDesc = desc;
                break;
            }
            default: break;
        }
        offset += featureHeader.length;
    }
    return tperDesc;
}
