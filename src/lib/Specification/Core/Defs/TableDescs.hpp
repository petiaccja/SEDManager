#include "../../Common/Utility.hpp"
#include "UIDs.hpp"

#include <array>
#include <span>


namespace sedmgr {

namespace core {

    inline auto TableDescs() {
        static constexpr std::array columnsTable = {
            ColumnDescStatic{"UID",         false, UID(eType::uid)                },
            ColumnDescStatic{ "Name",       true,  UID(eType::name)               },
            ColumnDescStatic{ "CommonName", true,  UID(eType::name)               },
            ColumnDescStatic{ "TemplateID", true,  UID(eType::Template_object_ref)},
            ColumnDescStatic{ "Kind",       false, UID(eType::table_kind)         },
            ColumnDescStatic{ "Column",     false, UID(eType::Column_object_ref)  },
            ColumnDescStatic{ "NumColumns", false, UID(eType::uinteger_4)         },
            ColumnDescStatic{ "Rows",       false, UID(eType::uinteger_4)         },
            ColumnDescStatic{ "RowsFree",   false, UID(eType::uinteger_4)         },
            ColumnDescStatic{ "RowBytes",   false, UID(eType::uinteger_4)         },
            ColumnDescStatic{ "LastID",     false, UID(eType::uid)                },
            ColumnDescStatic{ "MinSize",    false, UID(eType::uinteger_4)         },
            ColumnDescStatic{ "MaxSize",    false, UID(eType::uinteger_4)         },
        };

        static constexpr std::array columnsAuthority = {
            ColumnDescStatic{"UID",                 false, UID(eType::uid)                 },
            ColumnDescStatic{ "Name",               true,  UID(eType::name)                },
            ColumnDescStatic{ "CommonName",         true,  UID(eType::name)                },
            ColumnDescStatic{ "IsClass",            false, UID(eType::boolean)             },
            ColumnDescStatic{ "Class",              false, UID(eType::Authority_object_ref)},
            ColumnDescStatic{ "Enabled",            false, UID(eType::boolean)             },
            ColumnDescStatic{ "Secure",             false, UID(eType::messaging_type)      },
            ColumnDescStatic{ "HashAndSign",        false, UID(eType::hash_protocol)       },
            ColumnDescStatic{ "PresentCertificate", false, UID(eType::boolean)             },
            ColumnDescStatic{ "Operation",          false, UID(eType::auth_method)         },
            ColumnDescStatic{ "Credential",         false, UID(eType::cred_object_uidref)  },
            ColumnDescStatic{ "ResponseSign",       false, UID(eType::Authority_object_ref)},
            ColumnDescStatic{ "ResponseExch",       false, UID(eType::Authority_object_ref)},
            ColumnDescStatic{ "ClockStart",         false, UID(eType::date)                },
            ColumnDescStatic{ "ClockEnd",           false, UID(eType::date)                },
            ColumnDescStatic{ "Limit",              false, UID(eType::uinteger_4)          },
            ColumnDescStatic{ "Uses",               false, UID(eType::uinteger_4)          },
            ColumnDescStatic{ "Log",                false, UID(eType::log_select)          },
            ColumnDescStatic{ "LogTo",              false, UID(eType::LogList_object_ref)  },
        };

        static constexpr std::array columnsAccessControl = {
            ColumnDescStatic{"UID",              false, UID(eType::uid)                },
            ColumnDescStatic{ "InvokingID",      true,  UID(eType::table_or_object_ref)},
            ColumnDescStatic{ "MethodID",        true,  UID(eType::MethodID_object_ref)},
            ColumnDescStatic{ "CommonName",      false, UID(eType::name)               },
            ColumnDescStatic{ "ACL",             false, UID(eType::ACL)                },
            ColumnDescStatic{ "Log",             false, UID(eType::log_select)         },
            ColumnDescStatic{ "AddACEACL",       false, UID(eType::ACL)                },
            ColumnDescStatic{ "RemoveACEACL",    false, UID(eType::ACL)                },
            ColumnDescStatic{ "GetACLACL",       false, UID(eType::ACL)                },
            ColumnDescStatic{ "DeleteMethodACL", false, UID(eType::ACL)                },
            ColumnDescStatic{ "AddACELog",       false, UID(eType::log_select)         },
            ColumnDescStatic{ "RemoveACELog",    false, UID(eType::log_select)         },
            ColumnDescStatic{ "GetACLLog",       false, UID(eType::log_select)         },
            ColumnDescStatic{ "DeleteMethodLog", false, UID(eType::log_select)         },
            ColumnDescStatic{ "LogTo",           false, UID(eType::LogList_object_ref) },
        };

        static constexpr std::array columnsACE = {
            ColumnDescStatic{"UID",          false, UID(eType::uid)        },
            ColumnDescStatic{ "Name",        true,  UID(eType::name)       },
            ColumnDescStatic{ "CommonName",  true,  UID(eType::name)       },
            ColumnDescStatic{ "BooleanExpr", false, UID(eType::AC_element) },
            ColumnDescStatic{ "Columns",     false, UID(eType::ACE_columns)},
        };

        static constexpr std::array columnsMethodID = {
            ColumnDescStatic{"UID",         false, UID(eType::uid)                },
            ColumnDescStatic{ "Name",       true,  UID(eType::name)               },
            ColumnDescStatic{ "CommonName", true,  UID(eType::name)               },
            ColumnDescStatic{ "TemplateID", true,  UID(eType::Template_object_ref)},
        };

        static constexpr std::array columnsSecretProtect = {
            ColumnDescStatic{"UID",                false, UID(eType::uid)             },
            ColumnDescStatic{ "Table",             false, UID(eType::Table_object_ref)},
            ColumnDescStatic{ "ColumnNumber",      false, UID(eType::uinteger_4)      },
            ColumnDescStatic{ "ProtectMechanisms", false, UID(eType::protect_types)   },
        };

        static constexpr std::array columnsC_PIN = {
            ColumnDescStatic{"UID",          false, UID(eType::uid)           },
            ColumnDescStatic{ "Name",        true,  UID(eType::name)          },
            ColumnDescStatic{ "CommonName",  true,  UID(eType::name)          },
            ColumnDescStatic{ "PIN",         false, UID(eType::password)      },
            ColumnDescStatic{ "CharSet",     false, UID(eType::byte_table_ref)},
            ColumnDescStatic{ "TryLimit",    false, UID(eType::uinteger_4)    },
            ColumnDescStatic{ "Tries",       false, UID(eType::uinteger_4)    },
            ColumnDescStatic{ "Persistence", false, UID(eType::boolean)       },
        };

        static constexpr std::array columnsSP = {
            ColumnDescStatic{"UID",             false, UID(eType::uid)                 },
            ColumnDescStatic{ "Name",           true,  UID(eType::name)                },
            ColumnDescStatic{ "ORG",            false, UID(eType::Authority_object_ref)},
            ColumnDescStatic{ "EffectiveAuth",  false, UID(eType::max_bytes_32)        },
            ColumnDescStatic{ "DateofIssue",    false, UID(eType::date)                },
            ColumnDescStatic{ "Bytes",          false, UID(eType::uinteger_8)          },
            ColumnDescStatic{ "LifeCycleState", false, UID(eType::life_cycle_state)    },
            ColumnDescStatic{ "Frozen",         false, UID(eType::boolean)             },
        };

        static constexpr std::array columnsLocking = {
            ColumnDescStatic{"UID",               false, UID(eType::uid)                   },
            ColumnDescStatic{ "Name",             false, UID(eType::name)                  },
            ColumnDescStatic{ "CommonName",       false, UID(eType::name)                  },
            ColumnDescStatic{ "RangeStart",       false, UID(eType::uinteger_8)            },
            ColumnDescStatic{ "RangeLength",      false, UID(eType::uinteger_8)            },
            ColumnDescStatic{ "ReadLockEnabled",  false, UID(eType::boolean)               },
            ColumnDescStatic{ "WriteLockEnabled", false, UID(eType::boolean)               },
            ColumnDescStatic{ "ReadLocked",       false, UID(eType::boolean)               },
            ColumnDescStatic{ "WriteLocked",      false, UID(eType::boolean)               },
            ColumnDescStatic{ "LockOnReset",      false, UID(eType::reset_types)           },
            ColumnDescStatic{ "ActiveKey",        false, UID(eType::mediakey_object_uidref)},
            ColumnDescStatic{ "NextKey",          false, UID(eType::mediakey_object_uidref)},
            ColumnDescStatic{ "ReEncryptState",   false, UID(eType::reencrypt_state)       },
            ColumnDescStatic{ "ReEncryptRequest", false, UID(eType::reencrypt_request)     },
            ColumnDescStatic{ "AdvKeyMode",       false, UID(eType::adv_key_mode)          },
            ColumnDescStatic{ "VerifyMode",       false, UID(eType::verify_mode)           },
            ColumnDescStatic{ "ContOnReset",      false, UID(eType::reset_types)           },
            ColumnDescStatic{ "LastReEncryptLBA", false, UID(eType::uinteger_8)            },
            ColumnDescStatic{ "LastReEncStat",    false, UID(eType::last_reenc_stat)       },
            ColumnDescStatic{ "GeneralStatus",    false, UID(eType::gen_status)            },
        };

        static constexpr std::array columnsMBRControl = {
            ColumnDescStatic{"UID",             false, UID(eType::uid)        },
            ColumnDescStatic{ "Enable",         false, UID(eType::boolean)    },
            ColumnDescStatic{ "Done",           false, UID(eType::boolean)    },
            ColumnDescStatic{ "MBRDoneOnReset", false, UID(eType::reset_types)},
        };

        static constexpr std::array columnsLockingInfo = {
            ColumnDescStatic{"UID",               false, UID(eType::uid)             },
            ColumnDescStatic{ "Name",             false, UID(eType::name)            },
            ColumnDescStatic{ "Version",          false, UID(eType::uinteger_4)      },
            ColumnDescStatic{ "EncryptSupport",   false, UID(eType::enc_supported)   },
            ColumnDescStatic{ "MaxRanges",        false, UID(eType::uinteger_4)      },
            ColumnDescStatic{ "MaxReEncryptions", false, UID(eType::uinteger_4)      },
            ColumnDescStatic{ "KeysAvailableCfg", false, UID(eType::keys_avail_conds)},
        };


        static constexpr std::array columnsSPInfo = {
            ColumnDescStatic{"UID",               false, UID(eType::uid)       },
            ColumnDescStatic{ "SPID",             false, UID(eType::uid)       },
            ColumnDescStatic{ "Name",             false, UID(eType::name)      },
            ColumnDescStatic{ "Size",             false, UID(eType::uinteger_8)},
            ColumnDescStatic{ "SizeInUse",        false, UID(eType::uinteger_8)},
            ColumnDescStatic{ "SPSessionTimeout", false, UID(eType::uinteger_4)},
            ColumnDescStatic{ "Enabled",          false, UID(eType::boolean)   },
        };


        static constexpr std::array columnsTPerInfo = {
            ColumnDescStatic{"UID",               false, UID(eType::uid)       },
            ColumnDescStatic{ "Bytes",            false, UID(eType::uinteger_8)},
            ColumnDescStatic{ "GUDID",            false, UID(eType::bytes_12)  },
            ColumnDescStatic{ "Generation",       false, UID(eType::uinteger_4)},
            ColumnDescStatic{ "FirmwareVersion",  false, UID(eType::uinteger_4)},
            ColumnDescStatic{ "ProtocolVersion",  false, UID(eType::uinteger_4)},
            ColumnDescStatic{ "SpaceForIssuance", false, UID(eType::uinteger_8)},
            ColumnDescStatic{ "SSC",              false, UID(eType::SSC)       },
        };


        static constexpr std::array columnsSPTemplates = {
            ColumnDescStatic{"UID",         false, UID(eType::uid)                },
            ColumnDescStatic{ "TemplateID", false, UID(eType::Template_object_ref)},
            ColumnDescStatic{ "Name",       false, UID(eType::name)               },
            ColumnDescStatic{ "Version",    false, UID(eType::bytes_4)            }
        };


        static constexpr std::array columnsType = {
            ColumnDescStatic{"UID",         false, UID(eType::uid)       },
            ColumnDescStatic{ "Name",       true,  UID(eType::name)      },
            ColumnDescStatic{ "CommonName", true,  UID(eType::name)      },
            ColumnDescStatic{ "Format",     false, UID(eType::type_def)  },
            ColumnDescStatic{ "Size",       false, UID(eType::uinteger_2)},
        };


        static constexpr std::array columnsTemplate = {
            ColumnDescStatic{"UID",             false, UID(eType::uid)       },
            ColumnDescStatic{ "Name",           true,  UID(eType::name)      },
            ColumnDescStatic{ "RevisionNumber", false, UID(eType::uinteger_4)},
            ColumnDescStatic{ "Instances",      false, UID(eType::uinteger_2)},
            ColumnDescStatic{ "MaxInstances",   false, UID(eType::uinteger_2)},
        };


        static constexpr std::array columnsKAes128 = {
            ColumnDescStatic{"UID",         false, UID(eType::uid)                 },
            ColumnDescStatic{ "Name",       true,  UID(eType::name)                },
            ColumnDescStatic{ "CommonName", true,  UID(eType::name)                },
            ColumnDescStatic{ "Key",        false, UID(eType::key_128)             },
            ColumnDescStatic{ "Mode",       false, UID(eType::symmetric_mode_media)},
        };


        static constexpr std::array columnsKAes256 = {
            ColumnDescStatic{"UID",         false, UID(eType::uid)                 },
            ColumnDescStatic{ "Name",       true,  UID(eType::name)                },
            ColumnDescStatic{ "CommonName", true,  UID(eType::name)                },
            ColumnDescStatic{ "Key",        false, UID(eType::key_256)             },
            ColumnDescStatic{ "Mode",       false, UID(eType::symmetric_mode_media)},
        };


        static const std::initializer_list<std::pair<UID, TableDescStatic>> tableDescs = {
            {UID(eTable::Table),          TableDescStatic{ "Table", eTableKind::OBJECT, columnsTable }                                                },
            { UID(eTable::SPInfo),        TableDescStatic{ "SPInfo", eTableKind::OBJECT, columnsSPInfo, UID(eTableSingleRows::SPInfo) }               },
            { UID(eTable::SPTemplates),   TableDescStatic{ "SPTemplates", eTableKind::OBJECT, columnsSPTemplates }                                    },
            { UID(eTable::Column),        TableDescStatic{ "Column", eTableKind::OBJECT }                                                             },
            { UID(eTable::Type),          TableDescStatic{ "Type", eTableKind::OBJECT, columnsType }                                                  },
            { UID(eTable::MethodID),      TableDescStatic{ "MethodID", eTableKind::OBJECT, columnsMethodID }                                          },
            { UID(eTable::AccessControl), TableDescStatic{ "AccessControl", eTableKind::OBJECT, columnsAccessControl }                                },
            { UID(eTable::ACE),           TableDescStatic{ "ACE", eTableKind::OBJECT, columnsACE }                                                    },
            { UID(eTable::Authority),     TableDescStatic{ "Authority", eTableKind::OBJECT, columnsAuthority }                                        },
            { UID(eTable::Certificates),  TableDescStatic{ "Certificates", eTableKind::OBJECT }                                                       },
            { UID(eTable::C_PIN),         TableDescStatic{ "C_PIN", eTableKind::OBJECT, columnsC_PIN }                                                },
            { UID(eTable::C_RSA_1024),    TableDescStatic{ "C_RSA_1024", eTableKind::OBJECT }                                                         },
            { UID(eTable::C_RSA_2048),    TableDescStatic{ "C_RSA_2048", eTableKind::OBJECT }                                                         },
            { UID(eTable::C_AES_128),     TableDescStatic{ "C_AES_128", eTableKind::OBJECT }                                                          },
            { UID(eTable::C_AES_256),     TableDescStatic{ "C_AES_256", eTableKind::OBJECT }                                                          },
            { UID(eTable::C_EC_160),      TableDescStatic{ "C_EC_160", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_EC_192),      TableDescStatic{ "C_EC_192", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_EC_224),      TableDescStatic{ "C_EC_224", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_EC_256),      TableDescStatic{ "C_EC_256", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_EC_384),      TableDescStatic{ "C_EC_384", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_EC_521),      TableDescStatic{ "C_EC_521", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_EC_163),      TableDescStatic{ "C_EC_163", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_EC_233),      TableDescStatic{ "C_EC_233", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_EC_283),      TableDescStatic{ "C_EC_283", eTableKind::OBJECT }                                                           },
            { UID(eTable::C_HMAC_160),    TableDescStatic{ "C_HMAC_160", eTableKind::OBJECT }                                                         },
            { UID(eTable::C_HMAC_256),    TableDescStatic{ "C_HMAC_256", eTableKind::OBJECT }                                                         },
            { UID(eTable::C_HMAC_384),    TableDescStatic{ "C_HMAC_384", eTableKind::OBJECT }                                                         },
            { UID(eTable::C_HMAC_512),    TableDescStatic{ "C_HMAC_512", eTableKind::OBJECT }                                                         },
            { UID(eTable::SecretProtect), TableDescStatic{ "SecretProtect", eTableKind::OBJECT, columnsSecretProtect }                                },
            { UID(eTable::TPerInfo),      TableDescStatic{ "TPerInfo", eTableKind::OBJECT, columnsTPerInfo, UID(eTableSingleRows::TPerInfo) }         },
            { UID(eTable::CryptoSuite),   TableDescStatic{ "CryptoSuite", eTableKind::OBJECT }                                                        },
            { UID(eTable::Template),      TableDescStatic{ "Template", eTableKind::OBJECT, columnsTemplate }                                          },
            { UID(eTable::SP),            TableDescStatic{ "SP", eTableKind::OBJECT, columnsSP }                                                      },
            { UID(eTable::ClockTime),     TableDescStatic{ "ClockTime", eTableKind::OBJECT }                                                          },
            { UID(eTable::H_SHA_1),       TableDescStatic{ "H_SHA_1", eTableKind::OBJECT }                                                            },
            { UID(eTable::H_SHA_256),     TableDescStatic{ "H_SHA_256", eTableKind::OBJECT }                                                          },
            { UID(eTable::H_SHA_384),     TableDescStatic{ "H_SHA_384", eTableKind::OBJECT }                                                          },
            { UID(eTable::H_SHA_512),     TableDescStatic{ "H_SHA_512", eTableKind::OBJECT }                                                          },
            { UID(eTable::Log),           TableDescStatic{ "Log", eTableKind::OBJECT }                                                                },
            { UID(eTable::LogList),       TableDescStatic{ "LogList", eTableKind::OBJECT }                                                            },
            { UID(eTable::LockingInfo),   TableDescStatic{ "LockingInfo", eTableKind::OBJECT, columnsLockingInfo, UID(eTableSingleRows::LockingInfo) }},
            { UID(eTable::Locking),       TableDescStatic{ "Locking", eTableKind::OBJECT, columnsLocking }                                            },
            { UID(eTable::MBRControl),    TableDescStatic{ "MBRControl", eTableKind::OBJECT, columnsMBRControl, UID(eTableSingleRows::MBRControl) }   },
            { UID(eTable::MBR),           TableDescStatic{ "MBR", eTableKind::BYTE }                                                                  },
            { UID(eTable::K_AES_128),     TableDescStatic{ "K_AES_128", eTableKind::OBJECT, columnsKAes128 }                                          },
            { UID(eTable::K_AES_256),     TableDescStatic{ "K_AES_256", eTableKind::OBJECT, columnsKAes256 }                                          },
        };

        return tableDescs;
    }

} // namespace core
} // namespace sedmgr