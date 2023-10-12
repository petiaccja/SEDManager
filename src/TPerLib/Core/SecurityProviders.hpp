#pragma once

#include <cstdint>


enum class eSecurityProviderLifecycle {
    ISSUED = 0,
    DISABLED = 1,
    FROZEN = 2,
    DISABLED_FROZEN = 3,
    FAILED = 4,
};


namespace opal {

enum class eSecurityProvider : uint64_t {
    Admin = 0x0000'0205'0000'0001,
    Locking = 0x0000'0205'0000'0002,
};

enum class eSecurityProviderLifecycle {
    MANUFACTURED_INACTIVE = 8,
    MANUFACTURED = 9,
    MANUFACTURED_DISABLED = 10,
    MANUFACTURED_FROZEN = 11,
    MANUFACTURED_DISABLED_FROZEN = 12,
    MANUFACTURED_FAILED = 13,
};

} // namespace opal