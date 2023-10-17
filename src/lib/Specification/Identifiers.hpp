#pragma once

#include <cstdint>


enum class eTable : uint64_t {
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


enum class eTableSingleRowObject : uint64_t {
    SPInfo = 0x0000'0002'0000'0001,
    TPerInfo = 0x0000'0201'0000'0001,
    LockingInfo = 0x0000'0801'0000'0001,
    MBRControl = 0x0000'0803'0000'0001,
};


enum class eMethod : uint64_t {
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
};


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

enum class eMethod : uint64_t {
    Revert = static_cast<uint64_t>(::eMethod::Reserved_1),
    Activate = static_cast<uint64_t>(::eMethod::Reserved_2),
};


enum class eC_PIN : uint64_t {
    SID = 0x0000'000B'0000'0001,
    MSID = 0x0000'000B'0000'8402,
    Admin1 = 0x0000'000B'0000'0201,
    Admin2 = 0x0000'000B'0000'0202,
    Admin3 = 0x0000'000B'0000'0203,
    Admin4 = 0x0000'000B'0000'0204,
    Locking_Admin1 = 0x0000'000B'0001'0001,
    Locking_Admin2 = 0x0000'000B'0001'0002,
    Locking_Admin3 = 0x0000'000B'0001'0003,
    Locking_Admin4 = 0x0000'000B'0001'0004,
    User1 = 0x0000'000B'0003'0001,
    User2 = 0x0000'000B'0003'0002,
    User3 = 0x0000'000B'0003'0003,
    User4 = 0x0000'000B'0003'0004,
    User5 = 0x0000'000B'0003'0005,
    User6 = 0x0000'000B'0003'0006,
    User7 = 0x0000'000B'0003'0007,
    User8 = 0x0000'000B'0003'0008,
    User9 = 0x0000'000B'0003'0009,
    User10 = 0x0000'000B'0003'0010,
    User11 = 0x0000'000B'0003'0011,
    User12 = 0x0000'000B'0003'0012,
};


enum class eSecretProtect : uint64_t {
    K_AES_128 = 0x0000'001D'0000'001D,
    K_AES_256 = 0x0000'001D'0000'001E,
};

enum class eLocking : uint64_t {
    GlobalRange = 0x0000'0802'0000'0001,
    Range1 = 0x0000'0802'0003'0001,
    Range2 = 0x0000'0802'0003'0002,
    Range3 = 0x0000'0802'0003'0003,
    Range4 = 0x0000'0802'0003'0004,
    Range5 = 0x0000'0802'0003'0005,
    Range6 = 0x0000'0802'0003'0006,
    Range7 = 0x0000'0802'0003'0007,
    Range8 = 0x0000'0802'0003'0008,
    Range9 = 0x0000'0802'0003'0009,
    Range10 = 0x0000'0802'0003'0010,
    Range11 = 0x0000'0802'0003'0011,
    Range12 = 0x0000'0802'0003'0012,
};


enum class eACE : uint64_t {
    ACE_Anybody = 0x0000'0008'0000'0001,
    ACE_Admin = 0x0000'0008'0000'0002,
    ACE_Anybody_Get_CommonName = 0x0000'0008'0000'0003,
    ACE_Admins_Set_CommonName = 0x0000'0008'0000'0004,
    ACE_ACE_Get_All = 0x0000'0008'0003'8000,
    ACE_ACE_Set_BooleanExpression = 0x0000'0008'0003'8001,
    ACE_Authority_Get_All = 0x0000'0008'0003'9000,
    ACE_Authority_Set_Enabled = 0x0000'0008'0003'9001,
    ACE_User1_Set_CommonName = 0x0000'0008'0004'4001,
    ACE_User2_Set_CommonName = ACE_User1_Set_CommonName + 2,
    ACE_User3_Set_CommonName = ACE_User1_Set_CommonName + 3,
    ACE_User4_Set_CommonName = ACE_User1_Set_CommonName + 4,
    ACE_User5_Set_CommonName = ACE_User1_Set_CommonName + 5,
    ACE_User6_Set_CommonName = ACE_User1_Set_CommonName + 6,
    ACE_User7_Set_CommonName = ACE_User1_Set_CommonName + 7,
    ACE_User8_Set_CommonName = ACE_User1_Set_CommonName + 8,
    ACE_User9_Set_CommonName = ACE_User1_Set_CommonName + 9,
    ACE_User10_Set_CommonName = ACE_User1_Set_CommonName + 10,
    ACE_User11_Set_CommonName = ACE_User1_Set_CommonName + 11,
    ACE_User12_Set_CommonName = ACE_User1_Set_CommonName + 12,
    ACE_C_PIN_Admins_Get_All_NOPIN = 0x0000'0008'0003'A000,
    ACE_C_PIN_Admins_Set_PIN = 0x0000'0008'0003'A001,
    ACE_C_PIN_User1_Set_PIN = 0x0000'0008'0003'A801,
    ACE_C_PIN_User2_Set_PIN = ACE_C_PIN_User1_Set_PIN + 1,
    ACE_C_PIN_User3_Set_PIN = ACE_C_PIN_User1_Set_PIN + 2,
    ACE_C_PIN_User4_Set_PIN = ACE_C_PIN_User1_Set_PIN + 3,
    ACE_C_PIN_User5_Set_PIN = ACE_C_PIN_User1_Set_PIN + 4,
    ACE_C_PIN_User6_Set_PIN = ACE_C_PIN_User1_Set_PIN + 5,
    ACE_C_PIN_User7_Set_PIN = ACE_C_PIN_User1_Set_PIN + 6,
    ACE_C_PIN_User8_Set_PIN = ACE_C_PIN_User1_Set_PIN + 7,
    ACE_C_PIN_User9_Set_PIN = ACE_C_PIN_User1_Set_PIN + 8,
    ACE_C_PIN_User10_Set_PIN = ACE_C_PIN_User1_Set_PIN + 9,
    ACE_C_PIN_User11_Set_PIN = ACE_C_PIN_User1_Set_PIN + 10,
    ACE_C_PIN_User12_Set_PIN = ACE_C_PIN_User1_Set_PIN + 11,
    ACE_K_AES_Mode = 0x0000'0008'0003'BFFF,
    ACE_K_AES_128_GlobalRange_GenKey = 0x0000'0008'0003'B000,
    ACE_K_AES_128_Range1_GenKey = 0x0000'0008'0003'B001,
    ACE_K_AES_128_Range2_GenKey = ACE_K_AES_128_Range1_GenKey + 1,
    ACE_K_AES_128_Range3_GenKey = ACE_K_AES_128_Range1_GenKey + 2,
    ACE_K_AES_128_Range4_GenKey = ACE_K_AES_128_Range1_GenKey + 3,
    ACE_K_AES_128_Range5_GenKey = ACE_K_AES_128_Range1_GenKey + 4,
    ACE_K_AES_128_Range6_GenKey = ACE_K_AES_128_Range1_GenKey + 5,
    ACE_K_AES_128_Range7_GenKey = ACE_K_AES_128_Range1_GenKey + 6,
    ACE_K_AES_128_Range8_GenKey = ACE_K_AES_128_Range1_GenKey + 7,
    ACE_K_AES_128_Range9_GenKey = ACE_K_AES_128_Range1_GenKey + 8,
    ACE_K_AES_128_Range10_GenKey = ACE_K_AES_128_Range1_GenKey + 9,
    ACE_K_AES_128_Range11_GenKey = ACE_K_AES_128_Range1_GenKey + 10,
    ACE_K_AES_128_Range12_GenKey = ACE_K_AES_128_Range1_GenKey + 11,
    ACE_K_AES_256_GlobalRange_GenKey = 0x0000'0008'0003'B800,
    ACE_K_AES_256_Range1_GenKey = 0x0000'0008'0003'B801,
    ACE_K_AES_256_Range2_GenKey = ACE_K_AES_256_Range1_GenKey + 1,
    ACE_K_AES_256_Range3_GenKey = ACE_K_AES_256_Range1_GenKey + 2,
    ACE_K_AES_256_Range4_GenKey = ACE_K_AES_256_Range1_GenKey + 3,
    ACE_K_AES_256_Range5_GenKey = ACE_K_AES_256_Range1_GenKey + 4,
    ACE_K_AES_256_Range6_GenKey = ACE_K_AES_256_Range1_GenKey + 5,
    ACE_K_AES_256_Range7_GenKey = ACE_K_AES_256_Range1_GenKey + 6,
    ACE_K_AES_256_Range8_GenKey = ACE_K_AES_256_Range1_GenKey + 7,
    ACE_K_AES_256_Range9_GenKey = ACE_K_AES_256_Range1_GenKey + 8,
    ACE_K_AES_256_Range10_GenKey = ACE_K_AES_256_Range1_GenKey + 9,
    ACE_K_AES_256_Range11_GenKey = ACE_K_AES_256_Range1_GenKey + 10,
    ACE_K_AES_256_Range12_GenKey = ACE_K_AES_256_Range1_GenKey + 11,
    ACE_Locking_GlobalRange_Get_RangeStartToActiveKey = 0x0000'0008'0003'D000,
    ACE_Locking_Range1_Get_RangeStartToActiveKey = 0x0000'0008'0003'D001,
    ACE_Locking_Range2_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 1,
    ACE_Locking_Range3_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 2,
    ACE_Locking_Range4_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 3,
    ACE_Locking_Range5_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 4,
    ACE_Locking_Range6_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 5,
    ACE_Locking_Range7_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 6,
    ACE_Locking_Range8_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 7,
    ACE_Locking_Range9_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 8,
    ACE_Locking_Range10_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 9,
    ACE_Locking_Range11_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 10,
    ACE_Locking_Range12_Get_RangeStartToActiveKey = ACE_Locking_Range1_Get_RangeStartToActiveKey + 11,
    ACE_Locking_GlobalRange_Set_RdLocked = 0x0000'0008'0003'E000,
    ACE_Locking_Range1_Set_RdLocked = 0x0000'0008'0003'E001,
    ACE_Locking_Range2_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 1,
    ACE_Locking_Range3_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 2,
    ACE_Locking_Range4_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 3,
    ACE_Locking_Range5_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 4,
    ACE_Locking_Range6_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 5,
    ACE_Locking_Range7_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 6,
    ACE_Locking_Range8_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 7,
    ACE_Locking_Range9_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 8,
    ACE_Locking_Range10_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 9,
    ACE_Locking_Range11_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 10,
    ACE_Locking_Range12_Set_RdLocked = ACE_Locking_Range1_Set_RdLocked + 11,
    ACE_Locking_GlobalRange_Set_WrLocked = 0x0000'0008'0003'E800,
    ACE_Locking_Range1_Set_WrLocked = 0x0000'0008'0003'E801,
    ACE_Locking_Range2_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 1,
    ACE_Locking_Range3_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 2,
    ACE_Locking_Range4_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 3,
    ACE_Locking_Range5_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 4,
    ACE_Locking_Range6_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 5,
    ACE_Locking_Range7_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 6,
    ACE_Locking_Range8_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 7,
    ACE_Locking_Range9_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 8,
    ACE_Locking_Range10_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 9,
    ACE_Locking_Range11_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 10,
    ACE_Locking_Range12_Set_WrLocked = ACE_Locking_Range1_Set_WrLocked + 11,
    ACE_Locking_GlblRng_Admins_Set = 0x0000'0008'0003'F000,
    ACE_Locking_Admins_RangeStartToLOR = 0x0000'0008'0003'F001,
    ACE_MBRControl_Admins_Set = 0x0000'0008'0003'F800,
    ACE_MBRControl_Set_DoneToDOR = 0x0000'0008'0003'F801,
    ACE_DataStore_Get_All = 0x0000'0008'0003'FC00,
    ACE_DataStore_Set_All = 0x0000'0008'0003'FC01,
    ACE_Set_Enabled = 0x0000'0008'0003'0001,
    ACE_C_PIN_SID_Get_NOPIN = 0x0000'0008'0000'8C02,
    ACE_C_PIN_SID_Set_PIN = 0x0000'0008'0000'8C03,
    ACE_C_PIN_MSID_Get_PIN = 0x0000'0008'0000'8C04,
    ACE_TPerInfo_Set_ProgrammaticResetEnable = 0x0000'0008'0003'0003,
    ACE_SP_SID = 0x0000'0008'0003'0002,
    ACE_DataRemovalMechanism_Set_ActiveDataRemovalMechanism = 0x0000'0008'0005'0001,
};


enum class eSecurityProvider : uint64_t {
    Admin = 0x0000'0205'0000'0001,
    Locking = 0x0000'0205'0000'0002,
};


enum class eAuthority : uint64_t {
    PSID = 0x0000'0009'0001'FF01,
    Admin1 = 0x0000'0009'0001'0001,
    Admin2 = 0x0000'0009'0001'0002,
    Admin3 = 0x0000'0009'0001'0003,
    Admin4 = 0x0000'0009'0001'0004,
    Users = 0x0000'0009'0003'0000,
    User1 = 0x0000'0009'0003'0001,
    User2 = 0x0000'0009'0003'0002,
    User3 = 0x0000'0009'0003'0003,
    User4 = 0x0000'0009'0003'0004,
    User5 = 0x0000'0009'0003'0005,
    User6 = 0x0000'0009'0003'0006,
    User7 = 0x0000'0009'0003'0007,
    User8 = 0x0000'0009'0003'0008,
    User9 = 0x0000'0009'0003'0009,
    User10 = 0x0000'0009'0003'000A,
    User11 = 0x0000'0009'0003'000B,
    User12 = 0x0000'0009'0003'000C,
};

} // namespace opal