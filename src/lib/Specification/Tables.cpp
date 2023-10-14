#include "Tables.hpp"

#include <cassert>

namespace tables {

std::pair<const std::unordered_map<Uid, TableEntry>&, const std::unordered_map<Uid, ColumnEntry>&> TableDescriptions() {
    auto cache = []() -> std::pair<std::unordered_map<Uid, TableEntry>, std::unordered_map<Uid, ColumnEntry>> {
        auto tables = std::unordered_map<Uid, TableEntry>{
            {TableToDescriptor(eTable::Table),          { "Table", eTableKind::OBJECT, 0, 0 }                                          },
            { TableToDescriptor(eTable::SPInfo),        { "SPInfo", eTableKind::OBJECT, 0, 0, eTableSingleRowObject::SPInfo }          },
            { TableToDescriptor(eTable::SPTemplates),   { "SPTemplates", eTableKind::OBJECT, 0, 0 }                                    },
            { TableToDescriptor(eTable::Column),        { "Column", eTableKind::OBJECT, 0, 0 }                                         },
            { TableToDescriptor(eTable::Type),          { "Type", eTableKind::OBJECT, 0, 0 }                                           },
            { TableToDescriptor(eTable::MethodID),      { "MethodID", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::AccessControl), { "AccessControl", eTableKind::OBJECT, 0, 0 }                                  },
            { TableToDescriptor(eTable::ACE),           { "ACE", eTableKind::OBJECT, 0, 0 }                                            },
            { TableToDescriptor(eTable::Authority),     { "Authority", eTableKind::OBJECT, 0, 0 }                                      },
            { TableToDescriptor(eTable::Certificates),  { "Certificates", eTableKind::OBJECT, 0, 0 }                                   },
            { TableToDescriptor(eTable::C_PIN),         { "C_PIN", eTableKind::OBJECT, 0, 0 }                                          },
            { TableToDescriptor(eTable::C_RSA_1024),    { "C_RSA_1024", eTableKind::OBJECT, 0, 0 }                                     },
            { TableToDescriptor(eTable::C_RSA_2048),    { "C_RSA_2048", eTableKind::OBJECT, 0, 0 }                                     },
            { TableToDescriptor(eTable::C_AES_128),     { "C_AES_128", eTableKind::OBJECT, 0, 0 }                                      },
            { TableToDescriptor(eTable::C_AES_256),     { "C_AES_256", eTableKind::OBJECT, 0, 0 }                                      },
            { TableToDescriptor(eTable::C_EC_160),      { "C_EC_160", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_EC_192),      { "C_EC_192", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_EC_224),      { "C_EC_224", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_EC_256),      { "C_EC_256", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_EC_384),      { "C_EC_384", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_EC_521),      { "C_EC_521", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_EC_163),      { "C_EC_163", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_EC_233),      { "C_EC_233", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_EC_283),      { "C_EC_283", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::C_HMAC_160),    { "C_HMAC_160", eTableKind::OBJECT, 0, 0 }                                     },
            { TableToDescriptor(eTable::C_HMAC_256),    { "C_HMAC_256", eTableKind::OBJECT, 0, 0 }                                     },
            { TableToDescriptor(eTable::C_HMAC_384),    { "C_HMAC_384", eTableKind::OBJECT, 0, 0 }                                     },
            { TableToDescriptor(eTable::C_HMAC_512),    { "C_HMAC_512", eTableKind::OBJECT, 0, 0 }                                     },
            { TableToDescriptor(eTable::SecretProtect), { "SecretProtect", eTableKind::OBJECT, 0, 0 }                                  },
            { TableToDescriptor(eTable::TPerInfo),      { "TPerInfo", eTableKind::OBJECT, 0, 0, eTableSingleRowObject::TPerInfo }      },
            { TableToDescriptor(eTable::CryptoSuite),   { "CryptoSuite", eTableKind::OBJECT, 0, 0 }                                    },
            { TableToDescriptor(eTable::Template),      { "Template", eTableKind::OBJECT, 0, 0 }                                       },
            { TableToDescriptor(eTable::SP),            { "SP", eTableKind::OBJECT, 0, 0 }                                             },
            { TableToDescriptor(eTable::ClockTime),     { "ClockTime", eTableKind::OBJECT, 0, 0 }                                      },
            { TableToDescriptor(eTable::H_SHA_1),       { "H_SHA_1", eTableKind::OBJECT, 0, 0 }                                        },
            { TableToDescriptor(eTable::H_SHA_256),     { "H_SHA_256", eTableKind::OBJECT, 0, 0 }                                      },
            { TableToDescriptor(eTable::H_SHA_384),     { "H_SHA_384", eTableKind::OBJECT, 0, 0 }                                      },
            { TableToDescriptor(eTable::H_SHA_512),     { "H_SHA_512", eTableKind::OBJECT, 0, 0 }                                      },
            { TableToDescriptor(eTable::Log),           { "Log", eTableKind::OBJECT, 0, 0 }                                            },
            { TableToDescriptor(eTable::LogList),       { "LogList", eTableKind::OBJECT, 0, 0 }                                        },
            { TableToDescriptor(eTable::LockingInfo),   { "LockingInfo", eTableKind::OBJECT, 0, 0, eTableSingleRowObject::LockingInfo }},
            { TableToDescriptor(eTable::Locking),       { "Locking", eTableKind::OBJECT, 0, 0 }                                        },
            { TableToDescriptor(eTable::MBRControl),    { "MBRControl", eTableKind::OBJECT, 0, 0, eTableSingleRowObject::MBRControl }  },
            { TableToDescriptor(eTable::MBR),           { "MBR", eTableKind::BYTE, 0, 0 }                                              },
            { TableToDescriptor(eTable::K_AES_128),     { "K_AES_128", eTableKind::OBJECT, 0, 0 }                                      },
            { TableToDescriptor(eTable::K_AES_256),     { "K_AES_256", eTableKind::OBJECT, 0, 0 }                                      },
        };

        const std::initializer_list<ColumnEntry> columnsTable = {
            {0x00,  "UID",        false, "uid"                },
            { 0x01, "Name",       true,  "name"               },
            { 0x02, "CommonName", true,  "name"               },
            { 0x03, "TemplateID", true,  "Template_object_ref"},
            { 0x04, "Kind",       false, "table_kind"         },
            { 0x05, "Column",     false, "Column_object_ref"  },
            { 0x06, "NumColumns", false, "uinteger_4"         },
            { 0x07, "Rows",       false, "uinteger_4"         },
            { 0x08, "RowsFree",   false, "uinteger_4"         },
            { 0x09, "RowBytes",   false, "uinteger_4"         },
            { 0x0A, "LastID",     false, "uid"                },
            { 0x0B, "MinSize",    false, "uinteger_4"         },
            { 0x0C, "MaxSize",    false, "uinteger_4"         },
        };

        const std::initializer_list<ColumnEntry> columnsAuthority = {
            {0x00,  "UID",                false, "uid"                 },
            { 0x01, "Name",               true,  "name"                },
            { 0x02, "CommonName",         true,  "name"                },
            { 0x03, "IsClass",            false, "boolean"             },
            { 0x04, "Class",              false, "Authority_object_ref"},
            { 0x05, "Enabled",            false, "boolean"             },
            { 0x06, "Secure",             false, "messaging_type"      },
            { 0x07, "HashAndSign",        false, "hash_protocol"       },
            { 0x08, "PresentCertificate", false, "boolean"             },
            { 0x09, "Operation",          false, "auth_method"         },
            { 0x0A, "Credential",         false, "cred_object_uidref"  },
            { 0x0B, "ResponseSign",       false, "Authority_object_ref"},
            { 0x0C, "ResponseExch",       false, "Authority_object_ref"},
            { 0x0D, "ClockStart",         false, "date"                },
            { 0x0E, "ClockEnd",           false, "date"                },
            { 0x0F, "Limit",              false, "uinteger_4"          },
            { 0x10, "Uses",               false, "uinteger_4"          },
            { 0x11, "Log",                false, "log_select"          },
            { 0x12, "LogTo",              false, "LogList_object_ref"  },
        };

        const std::initializer_list<ColumnEntry> columnsAccessControl = {
            {0x00,  "UID",             false, "uid"                },
            { 0x01, "InvokingID",      true,  "table_or_object_ref"},
            { 0x02, "MethodID",        true,  "MethodID_object_ref"},
            { 0x03, "CommonName",      false, "name"               },
            { 0x04, "ACL",             false, "ACL"                },
            { 0x05, "Log",             false, "log_select"         },
            { 0x06, "AddACEACL",       false, "ACL"                },
            { 0x07, "RemoveACEACL",    false, "ACL"                },
            { 0x08, "GetACLACL",       false, "ACL"                },
            { 0x09, "DeleteMethodACL", false, "ACL"                },
            { 0x0A, "AddACELog",       false, "log_select"         },
            { 0x0B, "RemoveACELog",    false, "log_select"         },
            { 0x0C, "GetACLLog",       false, "log_select"         },
            { 0x0D, "DeleteMethodLog", false, "log_select"         },
            { 0x0E, "LogTo",           false, "LogList_object_ref" },
        };

        const std::initializer_list<ColumnEntry> columnsACE = {
            {0x00,  "UID",         false, "uid"        },
            { 0x01, "Name",        true,  "name"       },
            { 0x02, "CommonName",  true,  "name"       },
            { 0x03, "BooleanExpr", false, "AC_element" },
            { 0x04, "Columns",     false, "ACE_columns"},
        };

        const std::initializer_list<ColumnEntry> columnsMethodID = {
            {0x00,  "UID",        false, "uid"                },
            { 0x01, "Name",       true,  "name"               },
            { 0x02, "CommonName", true,  "name"               },
            { 0x03, "TemplateID", true,  "Template_object_ref"},
        };

        const std::initializer_list<ColumnEntry> columnsSecretProtect = {
            {0x00,  "UID",               false, "uid"             },
            { 0x01, "Table",             false, "Table_object_ref"},
            { 0x02, "ColumnNumber",      false, "uinteger_4"      },
            { 0x03, "ProtectMechanisms", false, "protect_types"   },
        };


        const std::initializer_list<ColumnEntry> columnsC_PIN = {
            {0x00,  "UID",         false, "uid"           },
            { 0x01, "Name",        true,  "name"          },
            { 0x02, "CommonName",  true,  "name"          },
            { 0x03, "PIN",         false, "password"      },
            { 0x04, "CharSet",     false, "byte_table_ref"},
            { 0x05, "TryLimit",    false, "uinteger_4"    },
            { 0x06, "Tries",       false, "uinteger_4"    },
            { 0x07, "Persistence", false, "boolean"       },
        };

        const std::initializer_list<ColumnEntry> columnsSP = {
            {0x00,  "UID",            false, "uid"                 },
            { 0x01, "Name",           true,  "name"                },
            { 0x02, "ORG",            false, "Authority_object_ref"},
            { 0x03, "EffectiveAuth",  false, "max_bytes_32"        },
            { 0x04, "DateofIssue",    false, "date"                },
            { 0x05, "Bytes",          false, "uinteger_8"          },
            { 0x06, "LifeCycleState", false, "life_cycle_state"    },
            { 0x07, "Frozen",         false, "boolean_def_false"   },
        };

        const std::initializer_list<ColumnEntry> columnsLocking = {
            {0x00,  "UID",              false, "uid"                   },
            { 0x01, "Name",             false, "name"                  },
            { 0x02, "CommonName",       false, "name"                  },
            { 0x03, "RangeStart",       false, "uinteger_8"            },
            { 0x04, "RangeLength",      false, "uinteger_8"            },
            { 0x05, "ReadLockEnabled",  false, "boolean"               },
            { 0x06, "WriteLockEnabled", false, "boolean"               },
            { 0x07, "ReadLocked",       false, "boolean"               },
            { 0x08, "WriteLocked",      false, "boolean"               },
            { 0x09, "LockOnReset",      false, "reset_types"           },
            { 0x0A, "ActiveKey",        false, "mediakey_object_uidref"},
            { 0x0B, "NextKey",          false, "mediakey_object_uidref"},
            { 0x0C, "ReEncryptState",   false, "reencrypt_state"       },
            { 0x0D, "ReEncryptRequest", false, "reencrypt_request"     },
            { 0x0E, "AdvKeyMode",       false, "adv_key_mode"          },
            { 0x0F, "VerifyMode",       false, "verify_mode"           },
            { 0x10, "ContOnReset",      false, "reset_types"           },
            { 0x11, "LastReEncryptLBA", false, "uinteger_8"            },
            { 0x12, "LastReEncStat",    false, "last_reenc_stat"       },
            { 0x13, "GeneralStatus",    false, "gen_status"            },
        };

        const std::initializer_list<ColumnEntry> columnsMBRControl = {
            {0x00,  "UID",            false, "uid"        },
            { 0x01, "Enable",         false, "boolean"    },
            { 0x02, "Done",           false, "boolean"    },
            { 0x03, "MBRDoneOnReset", false, "reset_types"},
        };

        const std::initializer_list<ColumnEntry> columnsLockingInfo = {
            {0x00,  "UID",              false, "uid"             },
            { 0x01, "Name",             false, "name"            },
            { 0x02, "Version",          false, "uinteger_4"      },
            { 0x03, "EncryptSupport",   false, "enc_supported"   },
            { 0x04, "MaxRanges",        false, "uinteger_4"      },
            { 0x05, "MaxReEncryptions", false, "uinteger_4"      },
            { 0x06, "KeysAvailableCfg", false, "keys_avail_conds"},
        };


        const std::initializer_list<ColumnEntry> columnsSPInfo = {
            {0x00,  "UID",              false, "uid"       },
            { 0x01, "SPID",             false, "uid"       },
            { 0x02, "Name",             false, "name"      },
            { 0x03, "Size",             false, "uinteger_8"},
            { 0x04, "SizeInUse",        false, "uinteger_8"},
            { 0x05, "SPSessionTimeout", false, "uinteger_4"},
            { 0x06, "Enabled",          false, "boolean"   },
        };


        auto amend = [](std::unordered_map<Uid, TableEntry>& tables, std::unordered_map<Uid, ColumnEntry>& columns, Uid table, const auto& tableColumns) {
            const auto it = tables.find(TableToDescriptor(table));
            if (it == tables.end()) {
                throw std::invalid_argument(std::format("there is no entry for table {:#018x}", uint64_t(table)));
            }
            auto& tableEntry = it->second;
            Uid* last = &tableEntry.column;
            Uid next = uint64_t(TableToDescriptor(table)) & 0x0000'0000'FFFF'FFFFull;
            for (const ColumnEntry& columnEntry : tableColumns) {
                *last = next;
                const auto [columnIt, inserted] = columns.insert({ next, columnEntry });
                assert(inserted);
                last = &columnIt->second.next;
                next = uint64_t(next) + (1ull << 32);
                tableEntry.numColumns += inserted;
            }
        };

        std::unordered_map<Uid, ColumnEntry> columns;
        amend(tables, columns, eTable::AccessControl, columnsAccessControl);
        amend(tables, columns, eTable::ACE, columnsACE);
        amend(tables, columns, eTable::Authority, columnsAuthority);
        amend(tables, columns, eTable::C_PIN, columnsC_PIN);
        amend(tables, columns, eTable::Locking, columnsLocking);
        amend(tables, columns, eTable::LockingInfo, columnsLockingInfo);
        amend(tables, columns, eTable::MBRControl, columnsMBRControl);
        amend(tables, columns, eTable::MethodID, columnsMethodID);
        amend(tables, columns, eTable::SecretProtect, columnsSecretProtect);
        amend(tables, columns, eTable::SP, columnsSP);
        amend(tables, columns, eTable::SPInfo, columnsSPInfo);
        amend(tables, columns, eTable::Table, columnsTable);

        return std::pair{ std::move(tables), std::move(columns) };
    };
    static const auto [table, column] = cache();
    return { table, column };
}


const std::unordered_map<Uid, TableEntry>& table = TableDescriptions().first;
const std::unordered_map<Uid, ColumnEntry>& column = TableDescriptions().second;


} // namespace tables