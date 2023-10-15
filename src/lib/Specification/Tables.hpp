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


constexpr Uid TableToDescriptor(Uid table) {
    return (uint64_t(table) >> 32) | (1ull << 32);
}


constexpr Uid DescriptorToTable(Uid descriptor) {
    return uint64_t(descriptor) << 32;
}


struct ColumnDesc {
    std::string name;
    bool isUnique;
    std::string type;
};


struct TableDesc {
    std::string name;
    eTableKind kind;
    std::vector<ColumnDesc> columns = {};
    std::optional<Uid> singleRow = std::nullopt;
};


TableDesc GetTableDesc(Uid table);