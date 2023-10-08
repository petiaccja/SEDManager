#pragma once

#include <cstdint>


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


namespace opal {

enum class eMethod : uint64_t {
    Revert = static_cast<uint64_t>(::eMethod::Reserved_1),
    Activate = static_cast<uint64_t>(::eMethod::Reserved_2),
};

}