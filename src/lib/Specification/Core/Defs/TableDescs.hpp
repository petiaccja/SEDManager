#include "../../Common/Utility.hpp"
#include "UIDs.hpp"

#include <array>
#include <span>


namespace sedmgr {

namespace core {

    inline auto TableDescs() {
        static constexpr std::array columnsTable = {
            ColumnDescStatic{"UID",         false, eType::uid                },
            ColumnDescStatic{ "Name",       true,  eType::name               },
            ColumnDescStatic{ "CommonName", true,  eType::name               },
            ColumnDescStatic{ "TemplateID", true,  eType::Template_object_ref},
            ColumnDescStatic{ "Kind",       false, eType::table_kind         },
            ColumnDescStatic{ "Column",     false, eType::Column_object_ref  },
            ColumnDescStatic{ "NumColumns", false, eType::uinteger_4         },
            ColumnDescStatic{ "Rows",       false, eType::uinteger_4         },
            ColumnDescStatic{ "RowsFree",   false, eType::uinteger_4         },
            ColumnDescStatic{ "RowBytes",   false, eType::uinteger_4         },
            ColumnDescStatic{ "LastID",     false, eType::uid                },
            ColumnDescStatic{ "MinSize",    false, eType::uinteger_4         },
            ColumnDescStatic{ "MaxSize",    false, eType::uinteger_4         },
        };

        static constexpr std::array columnsAuthority = {
            ColumnDescStatic{"UID",                 false, eType::uid                 },
            ColumnDescStatic{ "Name",               true,  eType::name                },
            ColumnDescStatic{ "CommonName",         true,  eType::name                },
            ColumnDescStatic{ "IsClass",            false, eType::boolean             },
            ColumnDescStatic{ "Class",              false, eType::Authority_object_ref},
            ColumnDescStatic{ "Enabled",            false, eType::boolean             },
            ColumnDescStatic{ "Secure",             false, eType::messaging_type      },
            ColumnDescStatic{ "HashAndSign",        false, eType::hash_protocol       },
            ColumnDescStatic{ "PresentCertificate", false, eType::boolean             },
            ColumnDescStatic{ "Operation",          false, eType::auth_method         },
            ColumnDescStatic{ "Credential",         false, eType::cred_object_uidref  },
            ColumnDescStatic{ "ResponseSign",       false, eType::Authority_object_ref},
            ColumnDescStatic{ "ResponseExch",       false, eType::Authority_object_ref},
            ColumnDescStatic{ "ClockStart",         false, eType::date                },
            ColumnDescStatic{ "ClockEnd",           false, eType::date                },
            ColumnDescStatic{ "Limit",              false, eType::uinteger_4          },
            ColumnDescStatic{ "Uses",               false, eType::uinteger_4          },
            ColumnDescStatic{ "Log",                false, eType::log_select          },
            ColumnDescStatic{ "LogTo",              false, eType::LogList_object_ref  },
        };

        static constexpr std::array columnsAccessControl = {
            ColumnDescStatic{"UID",              false, eType::uid                },
            ColumnDescStatic{ "InvokingID",      true,  eType::table_or_object_ref},
            ColumnDescStatic{ "MethodID",        true,  eType::MethodID_object_ref},
            ColumnDescStatic{ "CommonName",      false, eType::name               },
            ColumnDescStatic{ "ACL",             false, eType::ACL                },
            ColumnDescStatic{ "Log",             false, eType::log_select         },
            ColumnDescStatic{ "AddACEACL",       false, eType::ACL                },
            ColumnDescStatic{ "RemoveACEACL",    false, eType::ACL                },
            ColumnDescStatic{ "GetACLACL",       false, eType::ACL                },
            ColumnDescStatic{ "DeleteMethodACL", false, eType::ACL                },
            ColumnDescStatic{ "AddACELog",       false, eType::log_select         },
            ColumnDescStatic{ "RemoveACELog",    false, eType::log_select         },
            ColumnDescStatic{ "GetACLLog",       false, eType::log_select         },
            ColumnDescStatic{ "DeleteMethodLog", false, eType::log_select         },
            ColumnDescStatic{ "LogTo",           false, eType::LogList_object_ref },
        };

        static constexpr std::array columnsACE = {
            ColumnDescStatic{"UID",          false, eType::uid        },
            ColumnDescStatic{ "Name",        true,  eType::name       },
            ColumnDescStatic{ "CommonName",  true,  eType::name       },
            ColumnDescStatic{ "BooleanExpr", false, eType::AC_element },
            ColumnDescStatic{ "Columns",     false, eType::ACE_columns},
        };

        static constexpr std::array columnsMethodID = {
            ColumnDescStatic{"UID",         false, eType::uid                },
            ColumnDescStatic{ "Name",       true,  eType::name               },
            ColumnDescStatic{ "CommonName", true,  eType::name               },
            ColumnDescStatic{ "TemplateID", true,  eType::Template_object_ref},
        };

        static constexpr std::array columnsSecretProtect = {
            ColumnDescStatic{"UID",                false, eType::uid             },
            ColumnDescStatic{ "Table",             false, eType::Table_object_ref},
            ColumnDescStatic{ "ColumnNumber",      false, eType::uinteger_4      },
            ColumnDescStatic{ "ProtectMechanisms", false, eType::protect_types   },
        };

        static constexpr std::array columnsC_PIN = {
            ColumnDescStatic{"UID",          false, eType::uid           },
            ColumnDescStatic{ "Name",        true,  eType::name          },
            ColumnDescStatic{ "CommonName",  true,  eType::name          },
            ColumnDescStatic{ "PIN",         false, eType::password      },
            ColumnDescStatic{ "CharSet",     false, eType::byte_table_ref},
            ColumnDescStatic{ "TryLimit",    false, eType::uinteger_4    },
            ColumnDescStatic{ "Tries",       false, eType::uinteger_4    },
            ColumnDescStatic{ "Persistence", false, eType::boolean       },
        };

        static constexpr std::array columnsSP = {
            ColumnDescStatic{"UID",             false, eType::uid                 },
            ColumnDescStatic{ "Name",           true,  eType::name                },
            ColumnDescStatic{ "ORG",            false, eType::Authority_object_ref},
            ColumnDescStatic{ "EffectiveAuth",  false, eType::max_bytes_32        },
            ColumnDescStatic{ "DateofIssue",    false, eType::date                },
            ColumnDescStatic{ "Bytes",          false, eType::uinteger_8          },
            ColumnDescStatic{ "LifeCycleState", false, eType::life_cycle_state    },
            ColumnDescStatic{ "Frozen",         false, eType::boolean             },
        };

        static constexpr std::array columnsLocking = {
            ColumnDescStatic{"UID",               false, eType::uid                   },
            ColumnDescStatic{ "Name",             false, eType::name                  },
            ColumnDescStatic{ "CommonName",       false, eType::name                  },
            ColumnDescStatic{ "RangeStart",       false, eType::uinteger_8            },
            ColumnDescStatic{ "RangeLength",      false, eType::uinteger_8            },
            ColumnDescStatic{ "ReadLockEnabled",  false, eType::boolean               },
            ColumnDescStatic{ "WriteLockEnabled", false, eType::boolean               },
            ColumnDescStatic{ "ReadLocked",       false, eType::boolean               },
            ColumnDescStatic{ "WriteLocked",      false, eType::boolean               },
            ColumnDescStatic{ "LockOnReset",      false, eType::reset_types           },
            ColumnDescStatic{ "ActiveKey",        false, eType::mediakey_object_uidref},
            ColumnDescStatic{ "NextKey",          false, eType::mediakey_object_uidref},
            ColumnDescStatic{ "ReEncryptState",   false, eType::reencrypt_state       },
            ColumnDescStatic{ "ReEncryptRequest", false, eType::reencrypt_request     },
            ColumnDescStatic{ "AdvKeyMode",       false, eType::adv_key_mode          },
            ColumnDescStatic{ "VerifyMode",       false, eType::verify_mode           },
            ColumnDescStatic{ "ContOnReset",      false, eType::reset_types           },
            ColumnDescStatic{ "LastReEncryptLBA", false, eType::uinteger_8            },
            ColumnDescStatic{ "LastReEncStat",    false, eType::last_reenc_stat       },
            ColumnDescStatic{ "GeneralStatus",    false, eType::gen_status            },
        };

        static constexpr std::array columnsMBRControl = {
            ColumnDescStatic{"UID",             false, eType::uid        },
            ColumnDescStatic{ "Enable",         false, eType::boolean    },
            ColumnDescStatic{ "Done",           false, eType::boolean    },
            ColumnDescStatic{ "MBRDoneOnReset", false, eType::reset_types},
        };

        static constexpr std::array columnsLockingInfo = {
            ColumnDescStatic{"UID",               false, eType::uid             },
            ColumnDescStatic{ "Name",             false, eType::name            },
            ColumnDescStatic{ "Version",          false, eType::uinteger_4      },
            ColumnDescStatic{ "EncryptSupport",   false, eType::enc_supported   },
            ColumnDescStatic{ "MaxRanges",        false, eType::uinteger_4      },
            ColumnDescStatic{ "MaxReEncryptions", false, eType::uinteger_4      },
            ColumnDescStatic{ "KeysAvailableCfg", false, eType::keys_avail_conds},
        };


        static constexpr std::array columnsSPInfo = {
            ColumnDescStatic{"UID",               false, eType::uid       },
            ColumnDescStatic{ "SPID",             false, eType::uid       },
            ColumnDescStatic{ "Name",             false, eType::name      },
            ColumnDescStatic{ "Size",             false, eType::uinteger_8},
            ColumnDescStatic{ "SizeInUse",        false, eType::uinteger_8},
            ColumnDescStatic{ "SPSessionTimeout", false, eType::uinteger_4},
            ColumnDescStatic{ "Enabled",          false, eType::boolean   },
        };


        static constexpr std::array columnsTPerInfo = {
            ColumnDescStatic{"UID",               false, eType::uid       },
            ColumnDescStatic{ "Bytes",            false, eType::uinteger_8},
            ColumnDescStatic{ "GUDID",            false, eType::bytes_12  },
            ColumnDescStatic{ "Generation",       false, eType::uinteger_4},
            ColumnDescStatic{ "FirmwareVersion",  false, eType::uinteger_4},
            ColumnDescStatic{ "ProtocolVersion",  false, eType::uinteger_4},
            ColumnDescStatic{ "SpaceForIssuance", false, eType::uinteger_8},
            ColumnDescStatic{ "SSC",              false, eType::SSC       },
        };


        static constexpr std::array columnsSPTemplates = {
            ColumnDescStatic{"UID",         false, eType::uid                },
            ColumnDescStatic{ "TemplateID", false, eType::Template_object_ref},
            ColumnDescStatic{ "Name",       false, eType::name               },
            ColumnDescStatic{ "Version",    false, eType::bytes_4            }
        };


        static constexpr std::array columnsType = {
            ColumnDescStatic{"UID",         false, eType::uid       },
            ColumnDescStatic{ "Name",       true,  eType::name      },
            ColumnDescStatic{ "CommonName", true,  eType::name      },
            ColumnDescStatic{ "Format",     false, eType::type_def  },
            ColumnDescStatic{ "Size",       false, eType::uinteger_2},
        };


        static constexpr std::array columnsTemplate = {
            ColumnDescStatic{"UID",             false, eType::uid       },
            ColumnDescStatic{ "Name",           true,  eType::name      },
            ColumnDescStatic{ "RevisionNumber", false, eType::uinteger_4},
            ColumnDescStatic{ "Instances",      false, eType::uinteger_2},
            ColumnDescStatic{ "MaxInstances",   false, eType::uinteger_2},
        };


        static constexpr std::array columnsKAes128 = {
            ColumnDescStatic{"UID",         false, eType::uid                 },
            ColumnDescStatic{ "Name",       true,  eType::name                },
            ColumnDescStatic{ "CommonName", true,  eType::name                },
            ColumnDescStatic{ "Key",        false, eType::key_128             },
            ColumnDescStatic{ "Mode",       false, eType::symmetric_mode_media},
        };


        static constexpr std::array columnsKAes256 = {
            ColumnDescStatic{"UID",         false, eType::uid                 },
            ColumnDescStatic{ "Name",       true,  eType::name                },
            ColumnDescStatic{ "CommonName", true,  eType::name                },
            ColumnDescStatic{ "Key",        false, eType::key_256             },
            ColumnDescStatic{ "Mode",       false, eType::symmetric_mode_media},
        };


        static const std::initializer_list<std::pair<Uid, TableDescStatic>> tableDescs = {
            {eTable::Table,          TableDescStatic{ "Table", eTableKind::OBJECT, columnsTable }                                           },
            { eTable::SPInfo,        TableDescStatic{ "SPInfo", eTableKind::OBJECT, columnsSPInfo, eTableSingleRows::SPInfo }               },
            { eTable::SPTemplates,   TableDescStatic{ "SPTemplates", eTableKind::OBJECT, columnsSPTemplates }                               },
            { eTable::Column,        TableDescStatic{ "Column", eTableKind::OBJECT }                                                        },
            { eTable::Type,          TableDescStatic{ "Type", eTableKind::OBJECT, columnsType }                                             },
            { eTable::MethodID,      TableDescStatic{ "MethodID", eTableKind::OBJECT, columnsMethodID }                                     },
            { eTable::AccessControl, TableDescStatic{ "AccessControl", eTableKind::OBJECT, columnsAccessControl }                           },
            { eTable::ACE,           TableDescStatic{ "ACE", eTableKind::OBJECT, columnsACE }                                               },
            { eTable::Authority,     TableDescStatic{ "Authority", eTableKind::OBJECT, columnsAuthority }                                   },
            { eTable::Certificates,  TableDescStatic{ "Certificates", eTableKind::OBJECT }                                                  },
            { eTable::C_PIN,         TableDescStatic{ "C_PIN", eTableKind::OBJECT, columnsC_PIN }                                           },
            { eTable::C_RSA_1024,    TableDescStatic{ "C_RSA_1024", eTableKind::OBJECT }                                                    },
            { eTable::C_RSA_2048,    TableDescStatic{ "C_RSA_2048", eTableKind::OBJECT }                                                    },
            { eTable::C_AES_128,     TableDescStatic{ "C_AES_128", eTableKind::OBJECT }                                                     },
            { eTable::C_AES_256,     TableDescStatic{ "C_AES_256", eTableKind::OBJECT }                                                     },
            { eTable::C_EC_160,      TableDescStatic{ "C_EC_160", eTableKind::OBJECT }                                                      },
            { eTable::C_EC_192,      TableDescStatic{ "C_EC_192", eTableKind::OBJECT }                                                      },
            { eTable::C_EC_224,      TableDescStatic{ "C_EC_224", eTableKind::OBJECT }                                                      },
            { eTable::C_EC_256,      TableDescStatic{ "C_EC_256", eTableKind::OBJECT }                                                      },
            { eTable::C_EC_384,      TableDescStatic{ "C_EC_384", eTableKind::OBJECT }                                                      },
            { eTable::C_EC_521,      TableDescStatic{ "C_EC_521", eTableKind::OBJECT }                                                      },
            { eTable::C_EC_163,      TableDescStatic{ "C_EC_163", eTableKind::OBJECT }                                                      },
            { eTable::C_EC_233,      TableDescStatic{ "C_EC_233", eTableKind::OBJECT }                                                      },
            { eTable::C_EC_283,      TableDescStatic{ "C_EC_283", eTableKind::OBJECT }                                                      },
            { eTable::C_HMAC_160,    TableDescStatic{ "C_HMAC_160", eTableKind::OBJECT }                                                    },
            { eTable::C_HMAC_256,    TableDescStatic{ "C_HMAC_256", eTableKind::OBJECT }                                                    },
            { eTable::C_HMAC_384,    TableDescStatic{ "C_HMAC_384", eTableKind::OBJECT }                                                    },
            { eTable::C_HMAC_512,    TableDescStatic{ "C_HMAC_512", eTableKind::OBJECT }                                                    },
            { eTable::SecretProtect, TableDescStatic{ "SecretProtect", eTableKind::OBJECT, columnsSecretProtect }                           },
            { eTable::TPerInfo,      TableDescStatic{ "TPerInfo", eTableKind::OBJECT, columnsTPerInfo, eTableSingleRows::TPerInfo }         },
            { eTable::CryptoSuite,   TableDescStatic{ "CryptoSuite", eTableKind::OBJECT }                                                   },
            { eTable::Template,      TableDescStatic{ "Template", eTableKind::OBJECT, columnsTemplate }                                     },
            { eTable::SP,            TableDescStatic{ "SP", eTableKind::OBJECT, columnsSP }                                                 },
            { eTable::ClockTime,     TableDescStatic{ "ClockTime", eTableKind::OBJECT }                                                     },
            { eTable::H_SHA_1,       TableDescStatic{ "H_SHA_1", eTableKind::OBJECT }                                                       },
            { eTable::H_SHA_256,     TableDescStatic{ "H_SHA_256", eTableKind::OBJECT }                                                     },
            { eTable::H_SHA_384,     TableDescStatic{ "H_SHA_384", eTableKind::OBJECT }                                                     },
            { eTable::H_SHA_512,     TableDescStatic{ "H_SHA_512", eTableKind::OBJECT }                                                     },
            { eTable::Log,           TableDescStatic{ "Log", eTableKind::OBJECT }                                                           },
            { eTable::LogList,       TableDescStatic{ "LogList", eTableKind::OBJECT }                                                       },
            { eTable::LockingInfo,   TableDescStatic{ "LockingInfo", eTableKind::OBJECT, columnsLockingInfo, eTableSingleRows::LockingInfo }},
            { eTable::Locking,       TableDescStatic{ "Locking", eTableKind::OBJECT, columnsLocking }                                       },
            { eTable::MBRControl,    TableDescStatic{ "MBRControl", eTableKind::OBJECT, columnsMBRControl, eTableSingleRows::MBRControl }   },
            { eTable::MBR,           TableDescStatic{ "MBR", eTableKind::BYTE }                                                             },
            { eTable::K_AES_128,     TableDescStatic{ "K_AES_128", eTableKind::OBJECT, columnsKAes128 }                                     },
            { eTable::K_AES_256,     TableDescStatic{ "K_AES_256", eTableKind::OBJECT, columnsKAes256 }                                     },
        };

        return tableDescs;
    }

} // namespace core
} // namespace sedmgr