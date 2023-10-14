#pragma once

#include "Types.hpp"

#include <cstdint>
#include <unordered_map>


enum class eTableKind {
    OBJECT,
    BYTE,
};


enum class eTableSingleRowObject : uint64_t {
    SPInfo = 0x0000'0002'0000'0001,
    TPerInfo = 0x0000'0201'0000'0001,
    LockingInfo = 0x0000'0801'0000'0001,
    MBRControl = 0x0000'0803'0000'0001,
};


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


inline Uid TableToDescriptor(Uid table) {
    return (uint64_t(table) >> 32) | (1ull << 31);
}


inline Uid DescriptorToTable(Uid descriptor) {
    return uint64_t(descriptor) << 32;
}


enum class eRows_Locking : uint64_t {
    GlobalRange = 0x0000'0802'0000'0001,
};


enum class eColumns_SP {
    UID = 0x00, // uid
    Name = 0x01, // Yes name
    ORG = 0x02, // Authority_object_ref
    EffectiveAuth = 0x03, // max_bytes_32
    DateofIssue = 0x04, // date
    Bytes = 0x05, // uinteger_8
    LifeCycleState = 0x06, // life_cycle_state
    Frozen = 0x07, // boolean_def_false
};


enum class eColumns_Authority {
    UID = 0x00, // uid
    Name = 0x01, // Yes name
    CommonName = 0x02, // Yes name
    IsClass = 0x03, // boolean
    Class = 0x04, // Authority_object_ref
    Enabled = 0x05, // boolean
    Secure = 0x06, // messaging_type
    HashAndSign = 0x07, // hash_protocol
    PresentCertificate = 0x08, // boolean
    Operation = 0x09, // auth_method
    Credential = 0x0A, // cred_object_uidref
    ResponseSign = 0x0B, // Authority_object_ref
    ResponseExch = 0x0C, // Authority_object_ref
    ClockStart = 0x0D, // date
    ClockEnd = 0x0E, // date
    Limit = 0x0F, // uinteger_4
    Uses = 0x10, // uinteger_4
    Log = 0x11, // log_select
    LogTo = 0x12, // LogList_object_ref
};


enum class eColumns_Locking {
    UID = 0x00, // uid
    Name = 0x01, // name
    CommonName = 0x02, // name
    RangeStart = 0x03, // uinteger_8
    RangeLength = 0x04, // uinteger_8
    ReadLockEnabled = 0x05, // boolean
    WriteLockEnabled = 0x06, // boolean
    ReadLocked = 0x07, // boolean
    WriteLocked = 0x08, // boolean
    LockOnReset = 0x09, // reset_types
    ActiveKey = 0x0A, // mediakey_object_uidref
    NextKey = 0x0B, // mediakey_object_uidref
    ReEncryptState = 0x0C, // reencrypt_state
    ReEncryptRequest = 0x0D, // reencrypt_request
    AdvKeyMode = 0x0E, // adv_key_mode
    VerifyMode = 0x0F, // verify_mode
    ContOnReset = 0x10, // reset_types
    LastReEncryptLBA = 0x11, // uinteger_8
    LastReEncStat = 0x12, // last_reenc_stat
    GeneralStatus = 0x13, // gen_status
};


namespace opal {

enum class eRows_C_PIN : uint64_t {
    C_PIN_SID = 0x0000'000B'0000'0001,
    C_PIN_MSID = 0x0000'000B'0000'8402,
    C_PIN_Admin1 = 0x0000'000B'0000'0201,
};

enum class eRows_Locking : uint64_t {
    Range1 = 0x0000'0802'0003'0001,
    Range2 = 0x0000'0802'0003'0002,
    Range3 = 0x0000'0802'0003'0003,
    Range4 = 0x0000'0802'0003'0004,
    Range5 = 0x0000'0802'0003'0005,
    Range6 = 0x0000'0802'0003'0006,
    Range7 = 0x0000'0802'0003'0007,
    Range8 = 0x0000'0802'0003'0008,
};

} // namespace opal


namespace tables {


struct TableEntry {
    std::string name;
    eTableKind kind;
    Uid column = 0;
    uint32_t numColumns;
    std::optional<Uid> singleRow = std::nullopt;
};


struct ColumnEntry {
    uint32_t columnNumber;
    std::string_view name;
    bool isUnique;
    std::string_view type;
    Uid next = 0;
};


std::pair<const std::unordered_map<Uid, TableEntry>&, const std::unordered_map<Uid, ColumnEntry>&> TableDescriptions();

extern const std::unordered_map<Uid, TableEntry>& table;
extern const std::unordered_map<Uid, ColumnEntry>& column;


}; // namespace tables