#pragma once

#include <cstdint>


enum class eMethodId : uint64_t {
    // Session manager
    Properties = 0x0000'0000'0000'FF01,
    StartSession = 0x0000'0000'0000'FF02,
    SyncSession = 0x0000'0000'0000'FF03,
    StartTrustedSession = 0x0000'0000'0000'FF04,
    SyncTrustedSession = 0x0000'0000'0000'FF05,
    CloseSession = 0x0000'0000'0000'FF06,

    // Base template
    DeleteSP = 0x0000'0006'0000'0001,
    CreateTable = 0x0000'0006'0000'0002,
    Delete = 0x0000'0006'0000'0003,
    CreateRow = 0x0000'0006'0000'0004,
    DeleteRow = 0x0000'0006'0000'0005,
    Next = 0x0000'0006'0000'0008,
    GetFreeSpace = 0x0000'0006'0000'0009,
    GetFreeRows = 0x0000'0006'0000'000A,
    DeleteMethod = 0x0000'0006'0000'000B,
    GetACL = 0x0000'0006'0000'000D,
    AddACE = 0x0000'0006'0000'000E,
    RemoveACE = 0x0000'0006'0000'000F,
    GenKey = 0x0000'0006'0000'0010,
    GetPackage = 0x0000'0006'0000'0012,
    SetPackage = 0x0000'0006'0000'0013,
    Get = 0x0000'0006'0000'0016,
    Set = 0x0000'0006'0000'0017,
    Authenticate = 0x0000'0006'0000'001C,
    Obsolete_0 = 0x0000'0006'0000'0006,
    Obsolete_1 = 0x0000'0006'0000'0007,
    Obsolete_2 = 0x0000'0006'0000'000C,

    // Admin template
    IssueSP = 0x0000'0006'0000'0201,

    // Clock template
    GetClock = 0x0000'0006'0000'0401,
    ResetClock = 0x0000'0006'0000'0402,
    SetClockHigh = 0x0000'0006'0000'0403,
    SetLagHigh = 0x0000'0006'0000'0404,
    SetClockLow = 0x0000'0006'0000'0405,
    SetLagLow = 0x0000'0006'0000'0406,
    IncrementCounter = 0x0000'0006'0000'0407,

    // Crypto template
    Random = 0x0000'0006'0000'0601,
    Salt = 0x0000'0006'0000'0602,
    DecryptInit = 0x0000'0006'0000'0603,
    Decrypt = 0x0000'0006'0000'0604,
    DecryptFinalize = 0x0000'0006'0000'0605,
    EncryptInit = 0x0000'0006'0000'0606,
    Encrypt = 0x0000'0006'0000'0607,
    EncryptFinalize = 0x0000'0006'0000'0608,
    HMACInit = 0x0000'0006'0000'0609,
    HMAC = 0x0000'0006'0000'060A,
    HMACFinalize = 0x0000'0006'0000'060B,
    HashInit = 0x0000'0006'0000'060C,
    Hash = 0x0000'0006'0000'060D,
    HashFinalize = 0x0000'0006'0000'060E,
    Sign = 0x0000'0006'0000'060F,
    Verify = 0x0000'0006'0000'0610,
    XOR = 0x0000'0006'0000'0611,

    // Log template
    AddLog = 0x0000'0006'0000'0A01,
    CreateLog = 0x0000'0006'0000'0A02,
    ClearLog = 0x0000'0006'0000'0A03,
    FlushLog = 0x0000'0006'0000'0A04,

    // SSC-specific
    Reserved_0 = 0x0000'0006'0000'0011,
    Reserved_1 = 0x0000'0006'0000'0202,
    Reserved_2 = 0x0000'0006'0000'0203,
    Reserved_3 = 0x0000'0006'0000'0803,
    Opal_Revert = Reserved_1,
};


enum class eTableId : uint64_t {
    // Base
    Table = 0x0000'0001'0000'0000, // Base
    SPInfo = 0x0000'0002'0000'0000, // Base
    SPTemplates = 0x0000'0003'0000'0000, // Base
    Column = 0x0000'0004'0000'0000, // Base
    Type = 0x0000'0005'0000'0000, // Base
    MethodID = 0x0000'0006'0000'0000, // Base
    AccessControl = 0x0000'0007'0000'0000, // Base
    ACE = 0x0000'0008'0000'0000, // Base
    Authority = 0x0000'0009'0000'0000, // Base
    Certificates = 0x0000'000A'0000'0000, // Base
    C_PIN = 0x0000'000B'0000'0000, // Base
    C_RSA_1024 = 0x0000'000C'0000'0000, // Base
    C_RSA_2048 = 0x0000'000D'0000'0000, // Base
    C_AES_128 = 0x0000'000E'0000'0000, // Base
    C_AES_256 = 0x0000'000F'0000'0000, // Base
    C_EC_160 = 0x0000'0010'0000'0000, // Base
    C_EC_192 = 0x0000'0011'0000'0000, // Base
    C_EC_224 = 0x0000'0012'0000'0000, // Base
    C_EC_256 = 0x0000'0013'0000'0000, // Base
    C_EC_384 = 0x0000'0014'0000'0000, // Base
    C_EC_521 = 0x0000'0015'0000'0000, // Base
    C_EC_163 = 0x0000'0016'0000'0000, // Base
    C_EC_233 = 0x0000'0017'0000'0000, // Base
    C_EC_283 = 0x0000'0018'0000'0000, // Base
    C_HMAC_160 = 0x0000'0019'0000'0000, // Base
    C_HMAC_256 = 0x0000'001A'0000'0000, // Base
    C_HMAC_384 = 0x0000'001B'0000'0000, // Base
    C_HMAC_512 = 0x0000'001C'0000'0000, // Base
    SecretProtect = 0x0000'001D'0000'0000, // Base
    // Admin
    TPerInfo = 0x0000'0201'0000'0000, // Admin
    CryptoSuite = 0x0000'0203'0000'0000, // Admin
    Template = 0x0000'0204'0000'0000, // Admin
    SP = 0x0000'0205'0000'0000, // Admin
    // Clock
    ClockTime = 0x0000'0401'0000'0000, // Clock
    // Crypto
    H_SHA_1 = 0x0000'0601'0000'0000, // Crypto
    H_SHA_256 = 0x0000'0602'0000'0000, // Crypto
    H_SHA_384 = 0x0000'0603'0000'0000, // Crypto
    H_SHA_512 = 0x0000'0604'0000'0000, // Crypto
    // Log
    Log = 0x0000'0A01'0000'0000, // Log
    LogList = 0x0000'0A02'0000'0000, // Log
    // Locking
    LockingInfo = 0x0000'0801'0000'0000, // Locking
    Locking = 0x0000'0802'0000'0000, // Locking
    MBRControl = 0x0000'0803'0000'0000, // Locking
    MBR = 0x0000'0804'0000'0000, // Locking
    K_AES_128 = 0x0000'0805'0000'0000, // Locking
    K_AES_256 = 0x0000'0806'0000'0000, // Locking
};


enum class eTableDescriptorId : uint64_t {
    // Base
    Table = 0x0000'0001'0000'0001, // Base
    SPInfo = 0x0000'0001'0000'0002, // Base
    SPTemplates = 0x0000'0001'0000'0003, // Base
    Column = 0x0000'0001'0000'0004, // Base
    Type = 0x0000'0001'0000'0005, // Base
    MethodID = 0x0000'0001'0000'0006, // Base
    AccessControl = 0x0000'0001'0000'0007, // Base
    ACE = 0x0000'0001'0000'0008, // Base
    Authority = 0x0000'0001'0000'0009, // Base
    Certificates = 0x0000'0001'0000'000A, // Base
    C_PIN = 0x0000'0001'0000'000B, // Base
    C_RSA_1024 = 0x0000'0001'0000'000C, // Base
    C_RSA_2048 = 0x0000'0001'0000'000D, // Base
    C_AES_128 = 0x0000'0001'0000'000E, // Base
    C_AES_256 = 0x0000'0001'0000'000F, // Base
    C_EC_160 = 0x0000'0001'0000'0010, // Base
    C_EC_192 = 0x0000'0001'0000'0011, // Base
    C_EC_224 = 0x0000'0001'0000'0012, // Base
    C_EC_256 = 0x0000'0001'0000'0013, // Base
    C_EC_384 = 0x0000'0001'0000'0014, // Base
    C_EC_521 = 0x0000'0001'0000'0015, // Base
    C_EC_163 = 0x0000'0001'0000'0016, // Base
    C_EC_233 = 0x0000'0001'0000'0017, // Base
    C_EC_283 = 0x0000'0001'0000'0018, // Base
    C_HMAC_160 = 0x0000'0001'0000'0019, // Base
    C_HMAC_256 = 0x0000'0001'0000'001A, // Base
    C_HMAC_384 = 0x0000'0001'0000'001B, // Base
    C_HMAC_512 = 0x0000'0001'0000'001C, // Base
    SecretProtect = 0x0000'0001'0000'001D, // Base
    // Admin
    TPerInfo = 0x0000'0001'0000'0201, // Admin
    CryptoSuite = 0x0000'0001'0000'0203, // Admin
    Template = 0x0000'0001'0000'0204, // Admin
    SP = 0x0000'0001'0000'0205, // Admin
    // Clock
    ClockTime = 0x0000'0001'0000'0401, // Clock
    // Crypto
    H_SHA_1 = 0x0000'0001'0000'0601, // Crypto
    H_SHA_256 = 0x0000'0001'0000'0602, // Crypto
    H_SHA_384 = 0x0000'0001'0000'0603, // Crypto
    H_SHA_512 = 0x0000'0001'0000'0604, // Crypto
    // Log
    Log = 0x0000'0001'0000'0A01, // Log
    LogList = 0x0000'0001'0000'0A02, // Log
    // Locking
    LockingInfo = 0x0000'0001'0000'0801, // Locking
    Locking = 0x0000'0001'0000'0802, // Locking
    MBRControl = 0x0000'0001'0000'0803, // Locking
    MBR = 0x0000'0001'0000'0804, // Locking
    K_AES_128 = 0x0000'0001'0000'0805, // Locking
    K_AES_256 = 0x0000'0001'0000'0806, // Locking
};


enum class eAuthorityId : uint64_t {
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

enum class eTableRow_C_PIN : uint64_t {
    C_PIN_SID = 0x0000'000B'0000'0001,
    C_PIN_MSID = 0x0000'000B'0000'8402,
    C_PIN_Admin1 = 0x0000'000B'0000'0201,
};

enum class eSecurityProvider : uint64_t {
    Admin = 0x0000'0205'0000'0001,
    Locking = 0x0000'0205'0000'0002,
};

enum class eAuthorityId : uint64_t {
    PSID = 0x0000'0009'0001'FF01,
};

} // namespace opal