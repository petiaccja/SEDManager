#include "Tables.hpp"

#include <array>
#include <span>


struct ColumnDescStatic {
    std::string_view name;
    bool isUnique;
    std::string_view type;
};


struct TableDescStatic {
    Uid uid;
    std::string_view name;
    eTableKind kind;
    std::span<const ColumnDescStatic> columns = {};
    std::optional<Uid> singleRow = std::nullopt;
};


namespace impl {
namespace {

    constexpr std::array columnsTable = {
        ColumnDescStatic{"UID",         false, "uid"                },
        ColumnDescStatic{ "Name",       true,  "name"               },
        ColumnDescStatic{ "CommonName", true,  "name"               },
        ColumnDescStatic{ "TemplateID", true,  "Template_object_ref"},
        ColumnDescStatic{ "Kind",       false, "table_kind"         },
        ColumnDescStatic{ "Column",     false, "Column_object_ref"  },
        ColumnDescStatic{ "NumColumns", false, "uinteger_4"         },
        ColumnDescStatic{ "Rows",       false, "uinteger_4"         },
        ColumnDescStatic{ "RowsFree",   false, "uinteger_4"         },
        ColumnDescStatic{ "RowBytes",   false, "uinteger_4"         },
        ColumnDescStatic{ "LastID",     false, "uid"                },
        ColumnDescStatic{ "MinSize",    false, "uinteger_4"         },
        ColumnDescStatic{ "MaxSize",    false, "uinteger_4"         },
    };

    constexpr std::array columnsAuthority = {
        ColumnDescStatic{"UID",                 false, "uid"                 },
        ColumnDescStatic{ "Name",               true,  "name"                },
        ColumnDescStatic{ "CommonName",         true,  "name"                },
        ColumnDescStatic{ "IsClass",            false, "boolean"             },
        ColumnDescStatic{ "Class",              false, "Authority_object_ref"},
        ColumnDescStatic{ "Enabled",            false, "boolean"             },
        ColumnDescStatic{ "Secure",             false, "messaging_type"      },
        ColumnDescStatic{ "HashAndSign",        false, "hash_protocol"       },
        ColumnDescStatic{ "PresentCertificate", false, "boolean"             },
        ColumnDescStatic{ "Operation",          false, "auth_method"         },
        ColumnDescStatic{ "Credential",         false, "cred_object_uidref"  },
        ColumnDescStatic{ "ResponseSign",       false, "Authority_object_ref"},
        ColumnDescStatic{ "ResponseExch",       false, "Authority_object_ref"},
        ColumnDescStatic{ "ClockStart",         false, "date"                },
        ColumnDescStatic{ "ClockEnd",           false, "date"                },
        ColumnDescStatic{ "Limit",              false, "uinteger_4"          },
        ColumnDescStatic{ "Uses",               false, "uinteger_4"          },
        ColumnDescStatic{ "Log",                false, "log_select"          },
        ColumnDescStatic{ "LogTo",              false, "LogList_object_ref"  },
    };

    constexpr std::array columnsAccessControl = {
        ColumnDescStatic{"UID",              false, "uid"                },
        ColumnDescStatic{ "InvokingID",      true,  "table_or_object_ref"},
        ColumnDescStatic{ "MethodID",        true,  "MethodID_object_ref"},
        ColumnDescStatic{ "CommonName",      false, "name"               },
        ColumnDescStatic{ "ACL",             false, "ACL"                },
        ColumnDescStatic{ "Log",             false, "log_select"         },
        ColumnDescStatic{ "AddACEACL",       false, "ACL"                },
        ColumnDescStatic{ "RemoveACEACL",    false, "ACL"                },
        ColumnDescStatic{ "GetACLACL",       false, "ACL"                },
        ColumnDescStatic{ "DeleteMethodACL", false, "ACL"                },
        ColumnDescStatic{ "AddACELog",       false, "log_select"         },
        ColumnDescStatic{ "RemoveACELog",    false, "log_select"         },
        ColumnDescStatic{ "GetACLLog",       false, "log_select"         },
        ColumnDescStatic{ "DeleteMethodLog", false, "log_select"         },
        ColumnDescStatic{ "LogTo",           false, "LogList_object_ref" },
    };

    constexpr std::array columnsACE = {
        ColumnDescStatic{"UID",          false, "uid"        },
        ColumnDescStatic{ "Name",        true,  "name"       },
        ColumnDescStatic{ "CommonName",  true,  "name"       },
        ColumnDescStatic{ "BooleanExpr", false, "AC_element" },
        ColumnDescStatic{ "Columns",     false, "ACE_columns"},
    };

    constexpr std::array columnsMethodID = {
        ColumnDescStatic{"UID",         false, "uid"                },
        ColumnDescStatic{ "Name",       true,  "name"               },
        ColumnDescStatic{ "CommonName", true,  "name"               },
        ColumnDescStatic{ "TemplateID", true,  "Template_object_ref"},
    };

    constexpr std::array columnsSecretProtect = {
        ColumnDescStatic{"UID",                false, "uid"             },
        ColumnDescStatic{ "Table",             false, "Table_object_ref"},
        ColumnDescStatic{ "ColumnNumber",      false, "uinteger_4"      },
        ColumnDescStatic{ "ProtectMechanisms", false, "protect_types"   },
    };

    constexpr std::array columnsC_PIN = {
        ColumnDescStatic{"UID",          false, "uid"           },
        ColumnDescStatic{ "Name",        true,  "name"          },
        ColumnDescStatic{ "CommonName",  true,  "name"          },
        ColumnDescStatic{ "PIN",         false, "password"      },
        ColumnDescStatic{ "CharSet",     false, "byte_table_ref"},
        ColumnDescStatic{ "TryLimit",    false, "uinteger_4"    },
        ColumnDescStatic{ "Tries",       false, "uinteger_4"    },
        ColumnDescStatic{ "Persistence", false, "boolean"       },
    };

    constexpr std::array columnsSP = {
        ColumnDescStatic{"UID",             false, "uid"                 },
        ColumnDescStatic{ "Name",           true,  "name"                },
        ColumnDescStatic{ "ORG",            false, "Authority_object_ref"},
        ColumnDescStatic{ "EffectiveAuth",  false, "max_bytes_32"        },
        ColumnDescStatic{ "DateofIssue",    false, "date"                },
        ColumnDescStatic{ "Bytes",          false, "uinteger_8"          },
        ColumnDescStatic{ "LifeCycleState", false, "life_cycle_state"    },
        ColumnDescStatic{ "Frozen",         false, "boolean_def_false"   },
    };

    constexpr std::array columnsLocking = {
        ColumnDescStatic{"UID",               false, "uid"                   },
        ColumnDescStatic{ "Name",             false, "name"                  },
        ColumnDescStatic{ "CommonName",       false, "name"                  },
        ColumnDescStatic{ "RangeStart",       false, "uinteger_8"            },
        ColumnDescStatic{ "RangeLength",      false, "uinteger_8"            },
        ColumnDescStatic{ "ReadLockEnabled",  false, "boolean"               },
        ColumnDescStatic{ "WriteLockEnabled", false, "boolean"               },
        ColumnDescStatic{ "ReadLocked",       false, "boolean"               },
        ColumnDescStatic{ "WriteLocked",      false, "boolean"               },
        ColumnDescStatic{ "LockOnReset",      false, "reset_types"           },
        ColumnDescStatic{ "ActiveKey",        false, "mediakey_object_uidref"},
        ColumnDescStatic{ "NextKey",          false, "mediakey_object_uidref"},
        ColumnDescStatic{ "ReEncryptState",   false, "reencrypt_state"       },
        ColumnDescStatic{ "ReEncryptRequest", false, "reencrypt_request"     },
        ColumnDescStatic{ "AdvKeyMode",       false, "adv_key_mode"          },
        ColumnDescStatic{ "VerifyMode",       false, "verify_mode"           },
        ColumnDescStatic{ "ContOnReset",      false, "reset_types"           },
        ColumnDescStatic{ "LastReEncryptLBA", false, "uinteger_8"            },
        ColumnDescStatic{ "LastReEncStat",    false, "last_reenc_stat"       },
        ColumnDescStatic{ "GeneralStatus",    false, "gen_status"            },
    };

    constexpr std::array columnsMBRControl = {
        ColumnDescStatic{"UID",             false, "uid"        },
        ColumnDescStatic{ "Enable",         false, "boolean"    },
        ColumnDescStatic{ "Done",           false, "boolean"    },
        ColumnDescStatic{ "MBRDoneOnReset", false, "reset_types"},
    };

    constexpr std::array columnsLockingInfo = {
        ColumnDescStatic{"UID",               false, "uid"             },
        ColumnDescStatic{ "Name",             false, "name"            },
        ColumnDescStatic{ "Version",          false, "uinteger_4"      },
        ColumnDescStatic{ "EncryptSupport",   false, "enc_supported"   },
        ColumnDescStatic{ "MaxRanges",        false, "uinteger_4"      },
        ColumnDescStatic{ "MaxReEncryptions", false, "uinteger_4"      },
        ColumnDescStatic{ "KeysAvailableCfg", false, "keys_avail_conds"},
    };


    constexpr std::array columnsSPInfo = {
        ColumnDescStatic{"UID",               false, "uid"       },
        ColumnDescStatic{ "SPID",             false, "uid"       },
        ColumnDescStatic{ "Name",             false, "name"      },
        ColumnDescStatic{ "Size",             false, "uinteger_8"},
        ColumnDescStatic{ "SizeInUse",        false, "uinteger_8"},
        ColumnDescStatic{ "SPSessionTimeout", false, "uinteger_4"},
        ColumnDescStatic{ "Enabled",          false, "boolean"   },
    };

    constexpr auto tableDescs = std::array{
        TableDescStatic{ eTable::Table, "Table", eTableKind::OBJECT, columnsTable },
        TableDescStatic{ eTable::SPInfo, "SPInfo", eTableKind::OBJECT, columnsSPInfo, eTableSingleRowObject::SPInfo },
        TableDescStatic{ eTable::SPTemplates, "SPTemplates", eTableKind::OBJECT },
        TableDescStatic{ eTable::Column, "Column", eTableKind::OBJECT },
        TableDescStatic{ eTable::Type, "Type", eTableKind::OBJECT },
        TableDescStatic{ eTable::MethodID, "MethodID", eTableKind::OBJECT, columnsMethodID },
        TableDescStatic{ eTable::AccessControl, "AccessControl", eTableKind::OBJECT, columnsAccessControl },
        TableDescStatic{ eTable::ACE, "ACE", eTableKind::OBJECT, columnsACE },
        TableDescStatic{ eTable::Authority, "Authority", eTableKind::OBJECT, columnsAuthority },
        TableDescStatic{ eTable::Certificates, "Certificates", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_PIN, "C_PIN", eTableKind::OBJECT, columnsC_PIN },
        TableDescStatic{ eTable::C_RSA_1024, "C_RSA_1024", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_RSA_2048, "C_RSA_2048", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_AES_128, "C_AES_128", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_AES_256, "C_AES_256", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_160, "C_EC_160", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_192, "C_EC_192", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_224, "C_EC_224", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_256, "C_EC_256", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_384, "C_EC_384", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_521, "C_EC_521", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_163, "C_EC_163", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_233, "C_EC_233", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_EC_283, "C_EC_283", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_HMAC_160, "C_HMAC_160", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_HMAC_256, "C_HMAC_256", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_HMAC_384, "C_HMAC_384", eTableKind::OBJECT },
        TableDescStatic{ eTable::C_HMAC_512, "C_HMAC_512", eTableKind::OBJECT },
        TableDescStatic{ eTable::SecretProtect, "SecretProtect", eTableKind::OBJECT, columnsSecretProtect },
        TableDescStatic{ eTable::TPerInfo, "TPerInfo", eTableKind::OBJECT, {}, eTableSingleRowObject::TPerInfo },
        TableDescStatic{ eTable::CryptoSuite, "CryptoSuite", eTableKind::OBJECT },
        TableDescStatic{ eTable::Template, "Template", eTableKind::OBJECT },
        TableDescStatic{ eTable::SP, "SP", eTableKind::OBJECT, columnsSP },
        TableDescStatic{ eTable::ClockTime, "ClockTime", eTableKind::OBJECT },
        TableDescStatic{ eTable::H_SHA_1, "H_SHA_1", eTableKind::OBJECT },
        TableDescStatic{ eTable::H_SHA_256, "H_SHA_256", eTableKind::OBJECT },
        TableDescStatic{ eTable::H_SHA_384, "H_SHA_384", eTableKind::OBJECT },
        TableDescStatic{ eTable::H_SHA_512, "H_SHA_512", eTableKind::OBJECT },
        TableDescStatic{ eTable::Log, "Log", eTableKind::OBJECT },
        TableDescStatic{ eTable::LogList, "LogList", eTableKind::OBJECT },
        TableDescStatic{ eTable::LockingInfo, "LockingInfo", eTableKind::OBJECT, columnsLockingInfo, eTableSingleRowObject::LockingInfo },
        TableDescStatic{ eTable::Locking, "Locking", eTableKind::OBJECT, columnsLocking },
        TableDescStatic{ eTable::MBRControl, "MBRControl", eTableKind::OBJECT, columnsMBRControl, eTableSingleRowObject::MBRControl },
        TableDescStatic{ eTable::MBR, "MBR", eTableKind::BYTE },
        TableDescStatic{ eTable::K_AES_128, "K_AES_128", eTableKind::OBJECT },
        TableDescStatic{ eTable::K_AES_256, "K_AES_256", eTableKind::OBJECT },
    };

} // namespace
} // namespace impl


TableDesc GetTableDesc(Uid table) {
    const auto it = std::ranges::find_if(impl::tableDescs, [&](const auto& tableDesc) {
        return tableDesc.uid == table;
    });
    if (it != impl::tableDescs.end()) {
        std::vector<ColumnDesc> columns;
        for (auto& column : it->columns) {
            columns.emplace_back(std::string(column.name), column.isUnique, std::string(column.type));
        }
        return TableDesc{
            .name = std::string(it->name),
            .kind = it->kind,
            .columns = std::move(columns),
            .singleRow = it->singleRow,

        };
    }
    throw std::invalid_argument("could not find table description");
}
