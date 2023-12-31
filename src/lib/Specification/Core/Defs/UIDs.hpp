#pragma once


#include <cstdint>


namespace sedmgr {

namespace core {

    enum class eTable : uint64_t {
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
        TPerInfo = 0x0000'0201'0000'0000, // Admin
        CryptoSuite = 0x0000'0203'0000'0000, // Admin
        Template = 0x0000'0204'0000'0000, // Admin
        SP = 0x0000'0205'0000'0000, // Admin
        ClockTime = 0x0000'0401'0000'0000, // Clock
        H_SHA_1 = 0x0000'0601'0000'0000, // Crypto
        H_SHA_256 = 0x0000'0602'0000'0000, // Crypto
        H_SHA_384 = 0x0000'0603'0000'0000, // Crypto
        H_SHA_512 = 0x0000'0604'0000'0000, // Crypto
        Log = 0x0000'0A01'0000'0000, // Log
        LogList = 0x0000'0A02'0000'0000, // Log
        LockingInfo = 0x0000'0801'0000'0000, // Locking
        Locking = 0x0000'0802'0000'0000, // Locking
        MBRControl = 0x0000'0803'0000'0000, // Locking
        MBR = 0x0000'0804'0000'0000, // Locking
        K_AES_128 = 0x0000'0805'0000'0000, // Locking
        K_AES_256 = 0x0000'0806'0000'0000, // Locking
    };


    enum class eTableSingleRows : uint64_t {
        SPInfo = 0x0000'0002'0000'0001,
        TPerInfo = 0x0000'0201'0000'0001,
        LockingInfo = 0x0000'0801'0000'0001,
        MBRControl = 0x0000'0803'0000'0001,
    };


    enum class eMethod : uint64_t {
        Properties = 0x0000'0000'0000'FF01,
        StartSession = 0x0000'0000'0000'FF02,
        SyncSession = 0x0000'0000'0000'FF03,
        StartTrustedSession = 0x0000'0000'0000'FF04,
        SyncTrustedSession = 0x0000'0000'0000'FF05,
        CloseSession = 0x0000'0000'0000'FF06,
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
        IssueSP = 0x0000'0006'0000'0201,
        GetClock = 0x0000'0006'0000'0401,
        ResetClock = 0x0000'0006'0000'0402,
        SetClockHigh = 0x0000'0006'0000'0403,
        SetLagHigh = 0x0000'0006'0000'0404,
        SetClockLow = 0x0000'0006'0000'0405,
        SetLagLow = 0x0000'0006'0000'0406,
        IncrementCounter = 0x0000'0006'0000'0407,
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
        AddLog = 0x0000'0006'0000'0A01,
        CreateLog = 0x0000'0006'0000'0A02,
        ClearLog = 0x0000'0006'0000'0A03,
        FlushLog = 0x0000'0006'0000'0A04,
        Reserved_0 = 0x0000'0006'0000'0011,
        Reserved_1 = 0x0000'0006'0000'0202,
        Reserved_2 = 0x0000'0006'0000'0203,
        Reserved_3 = 0x0000'0006'0000'0803,
    };


    enum class eType : uint64_t {
        unknown_type = 0x0000'0005'FFFF'FFFF,
        boolean = 0x0000'0005'0000'0401,
        integer_1 = 0x0000'0005'0000'0210,
        integer_2 = 0x0000'0005'0000'0213,
        uinteger_1 = 0x0000'0005'0000'0211,
        uinteger_2 = 0x0000'0005'0000'0215,
        uinteger_4 = 0x0000'0005'0000'0220,
        uinteger_8 = 0x0000'0005'0000'0225,
        max_bytes_32 = 0x0000'0005'0000'020D,
        max_bytes_64 = 0x0000'0005'0000'020E,
        bytes_4 = 0x0000'0005'0000'0238,
        bytes_12 = 0x0000'0005'0000'0201,
        bytes_16 = 0x0000'0005'0000'0202,
        bytes_32 = 0x0000'0005'0000'0205,
        bytes_64 = 0x0000'0005'0000'0206,
        key_128 = 0x0000'0005'0000'0602,
        key_256 = 0x0000'0005'0000'0603,
        type_def = 0x0000'0005'0000'0203,
        name = 0x0000'0005'0000'020B,
        password = 0x0000'0005'0000'020C,
        year_enum = 0x0000'0005'0000'0416,
        month_enum = 0x0000'0005'0000'0417,
        day_enum = 0x0000'0005'0000'0418,
        Year = 0x0000'0005'0000'1401,
        Month = 0x0000'0005'0000'1402,
        Day = 0x0000'0005'0000'1403,
        date = 0x0000'0005'0000'1804,
        messaging_type = 0x0000'0005'0000'0404,
        hash_protocol = 0x0000'0005'0000'040D,
        auth_method = 0x0000'0005'0000'0408,
        log_select = 0x0000'0005'0000'040C,
        protect_types = 0x0000'0005'0000'1A05,
        reencrypt_request = 0x0000'0005'0000'0413,
        reencrypt_state = 0x0000'0005'0000'0414,
        reset_types = 0x0000'0005'0000'1A01,
        adv_key_mode = 0x0000'0005'0000'040F,
        verify_mode = 0x0000'0005'0000'0412,
        last_reenc_stat = 0x0000'0005'0000'0411,
        gen_status = 0x0000'0005'0000'1A02,
        enc_supported = 0x0000'0005'0000'041D,
        keys_avail_conds = 0x0000'0005'0000'0410,
        symmetric_mode_media = 0x0000'0005'0000'0403,
        uid = 0x0000'0005'0000'0209,
        table_kind = 0x0000'0005'0000'0415,
        object_ref = 0x0000'0005'0000'0F02,
        table_ref = 0x0000'0005'0000'0F03,
        byte_table_ref = 0x0000'0005'0000'1001,
        table_or_object_ref = 0x0000'0005'0000'0606,
        Authority_object_ref = 0x0000'0005'0000'0C05,
        Table_object_ref = 0x0000'0005'0000'0C09,
        MethodID_object_ref = 0x0000'0005'0000'0C03,
        SPTemplates_object = 0x0000'0005'0000'0C01,
        Column_object_ref = 0x0000'0005'0000'0C07,
        Template_object_ref = 0x0000'0005'0000'0C08,
        LogList_object_ref = 0x0000'0005'0000'0C0D,
        cred_object_uidref = 0x0000'0005'0000'0C0B,
        mediakey_object_uidref = 0x0000'0005'0000'0C0C,
        boolean_ACE = 0x0000'0005'0000'040E,
        ACE_expression = 0x0000'0005'0000'0601,
        AC_element = 0x0000'0005'0000'0801,
        ACE_object_ref = 0x0000'0005'0000'0C04,
        ACL = 0x0000'0005'0000'0802,
        ACE_columns = 0x0000'0005'0000'1A03,
        life_cycle_state = 0x0000'0005'0000'0405,
        SSC = 0x0000'0005'0000'0803,
    };

} // namespace core
} // namespace sedmgr