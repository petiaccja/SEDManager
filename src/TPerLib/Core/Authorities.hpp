#pragma once

#include <cstdint>


enum class eAuthority : uint64_t {
    // Base
    Anybody = 0x0000'0009'0000'0001, // Base
    Admins = 0x0000'0009'0000'0002, // Base
    Makers = 0x0000'0009'0000'0003, // Base
    MakerSymK = 0x0000'0009'0000'0004, // Base
    MakerPuK = 0x0000'0009'0000'0005, // Base
    SID = 0x0000'0009'0000'0006, // Base
    TPerSign = 0x0000'0009'0000'0007, // Base
    TPerExch = 0x0000'0009'0000'0008, // Base
    AdminExch = 0x0000'0009'0000'0009, // Base
    // Admin
    Issuers = 0x0000'0009'0000'0201, // Admin
    Editors = 0x0000'0009'0000'0202, // Admin
    Deleters = 0x0000'0009'0000'0203, // Admin
    Servers = 0x0000'0009'0000'0204, // Admin
    Reserve0 = 0x0000'0009'0000'0205, // Admin
    Reserve1 = 0x0000'0009'0000'0206, // Admin
    Reserve2 = 0x0000'0009'0000'0207, // Admin
    Reserve3 = 0x0000'0009'0000'0208, // Admin
};


namespace opal {

enum class eAuthority : uint64_t {
    PSID = 0x0000'0009'0001'FF01,
    Admin1 = 0x0000'0009'0001'0001,
    Admin2 = 0x0000'0009'0001'0002,
    Admin3 = 0x0000'0009'0001'0003,
    Admin4 = 0x0000'0009'0001'0004,
    User1 = 0x0000'0009'0003'0001,
    User2 = 0x0000'0009'0003'0002,
    User3 = 0x0000'0009'0003'0003,
    User4 = 0x0000'0009'0003'0004,
    User5 = 0x0000'0009'0003'0005,
    User6 = 0x0000'0009'0003'0006,
    User7 = 0x0000'0009'0003'0007,
    User8 = 0x0000'0009'0003'0008,
};

} // namespace opal