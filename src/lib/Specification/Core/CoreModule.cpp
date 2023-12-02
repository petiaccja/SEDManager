#include "CoreModule.hpp"

#include "../Common/Utility.hpp"
#include "Defs/TableDescs.hpp"
#include "Defs/TypeDefs.hpp"
#include "Defs/UIDs.hpp"

#include <algorithm>


namespace sedmgr {

namespace core {

    constexpr std::initializer_list<std::pair<Uid, std::string_view>> tables = {
        {eTable::Table,          "Table"        }, // Base
        { eTable::SPInfo,        "SPInfo"       }, // Base
        { eTable::SPTemplates,   "SPTemplates"  }, // Base
        { eTable::Column,        "Column"       }, // Base
        { eTable::Type,          "Type"         }, // Base
        { eTable::MethodID,      "MethodID"     }, // Base
        { eTable::AccessControl, "AccessControl"}, // Base
        { eTable::ACE,           "ACE"          }, // Base
        { eTable::Authority,     "Authority"    }, // Base
        { eTable::Certificates,  "Certificates" }, // Base
        { eTable::C_PIN,         "C_PIN"        }, // Base
        { eTable::C_RSA_1024,    "C_RSA_1024"   }, // Base
        { eTable::C_RSA_2048,    "C_RSA_2048"   }, // Base
        { eTable::C_AES_128,     "C_AES_128"    }, // Base
        { eTable::C_AES_256,     "C_AES_256"    }, // Base
        { eTable::C_EC_160,      "C_EC_160"     }, // Base
        { eTable::C_EC_192,      "C_EC_192"     }, // Base
        { eTable::C_EC_224,      "C_EC_224"     }, // Base
        { eTable::C_EC_256,      "C_EC_256"     }, // Base
        { eTable::C_EC_384,      "C_EC_384"     }, // Base
        { eTable::C_EC_521,      "C_EC_521"     }, // Base
        { eTable::C_EC_163,      "C_EC_163"     }, // Base
        { eTable::C_EC_233,      "C_EC_233"     }, // Base
        { eTable::C_EC_283,      "C_EC_283"     }, // Base
        { eTable::C_HMAC_160,    "C_HMAC_160"   }, // Base
        { eTable::C_HMAC_256,    "C_HMAC_256"   }, // Base
        { eTable::C_HMAC_384,    "C_HMAC_384"   }, // Base
        { eTable::C_HMAC_512,    "C_HMAC_512"   }, // Base
        { eTable::SecretProtect, "SecretProtect"}, // Base
        { eTable::TPerInfo,      "TPerInfo"     }, // Admin
        { eTable::CryptoSuite,   "CryptoSuite"  }, // Admin
        { eTable::Template,      "Template"     }, // Admin
        { eTable::SP,            "SP"           }, // Admin
        { eTable::ClockTime,     "ClockTime"    }, // Clock
        { eTable::H_SHA_1,       "H_SHA_1"      }, // Crypto
        { eTable::H_SHA_256,     "H_SHA_256"    }, // Crypto
        { eTable::H_SHA_384,     "H_SHA_384"    }, // Crypto
        { eTable::H_SHA_512,     "H_SHA_512"    }, // Crypto
        { eTable::Log,           "Log"          }, // Log
        { eTable::LogList,       "LogList"      }, // Log
        { eTable::LockingInfo,   "LockingInfo"  }, // Locking
        { eTable::Locking,       "Locking"      }, // Locking
        { eTable::MBRControl,    "MBRControl"   }, // Locking
        { eTable::MBR,           "MBR"          }, // Locking
        { eTable::K_AES_128,     "K_AES_128"    }, // Locking
        { eTable::K_AES_256,     "K_AES_256"    }, // Locking
    };


    constexpr std::initializer_list<std::pair<Uid, std::string_view>> tablesDescriptors = {
        {TableToDescriptor(eTable::Table),          "Table::Table"        }, // Base
        { TableToDescriptor(eTable::SPInfo),        "Table::SPInfo"       }, // Base
        { TableToDescriptor(eTable::SPTemplates),   "Table::SPTemplates"  }, // Base
        { TableToDescriptor(eTable::Column),        "Table::Column"       }, // Base
        { TableToDescriptor(eTable::Type),          "Table::Type"         }, // Base
        { TableToDescriptor(eTable::MethodID),      "Table::MethodID"     }, // Base
        { TableToDescriptor(eTable::AccessControl), "Table::AccessControl"}, // Base
        { TableToDescriptor(eTable::ACE),           "Table::ACE"          }, // Base
        { TableToDescriptor(eTable::Authority),     "Table::Authority"    }, // Base
        { TableToDescriptor(eTable::Certificates),  "Table::Certificates" }, // Base
        { TableToDescriptor(eTable::C_PIN),         "Table::C_PIN"        }, // Base
        { TableToDescriptor(eTable::C_RSA_1024),    "Table::C_RSA_1024"   }, // Base
        { TableToDescriptor(eTable::C_RSA_2048),    "Table::C_RSA_2048"   }, // Base
        { TableToDescriptor(eTable::C_AES_128),     "Table::C_AES_128"    }, // Base
        { TableToDescriptor(eTable::C_AES_256),     "Table::C_AES_256"    }, // Base
        { TableToDescriptor(eTable::C_EC_160),      "Table::C_EC_160"     }, // Base
        { TableToDescriptor(eTable::C_EC_192),      "Table::C_EC_192"     }, // Base
        { TableToDescriptor(eTable::C_EC_224),      "Table::C_EC_224"     }, // Base
        { TableToDescriptor(eTable::C_EC_256),      "Table::C_EC_256"     }, // Base
        { TableToDescriptor(eTable::C_EC_384),      "Table::C_EC_384"     }, // Base
        { TableToDescriptor(eTable::C_EC_521),      "Table::C_EC_521"     }, // Base
        { TableToDescriptor(eTable::C_EC_163),      "Table::C_EC_163"     }, // Base
        { TableToDescriptor(eTable::C_EC_233),      "Table::C_EC_233"     }, // Base
        { TableToDescriptor(eTable::C_EC_283),      "Table::C_EC_283"     }, // Base
        { TableToDescriptor(eTable::C_HMAC_160),    "Table::C_HMAC_160"   }, // Base
        { TableToDescriptor(eTable::C_HMAC_256),    "Table::C_HMAC_256"   }, // Base
        { TableToDescriptor(eTable::C_HMAC_384),    "Table::C_HMAC_384"   }, // Base
        { TableToDescriptor(eTable::C_HMAC_512),    "Table::C_HMAC_512"   }, // Base
        { TableToDescriptor(eTable::SecretProtect), "Table::SecretProtect"}, // Base
        { TableToDescriptor(eTable::TPerInfo),      "Table::TPerInfo"     }, // Admin
        { TableToDescriptor(eTable::CryptoSuite),   "Table::CryptoSuite"  }, // Admin
        { TableToDescriptor(eTable::Template),      "Table::Template"     }, // Admin
        { TableToDescriptor(eTable::SP),            "Table::SP"           }, // Admin
        { TableToDescriptor(eTable::ClockTime),     "Table::ClockTime"    }, // Clock
        { TableToDescriptor(eTable::H_SHA_1),       "Table::H_SHA_1"      }, // Crypto
        { TableToDescriptor(eTable::H_SHA_256),     "Table::H_SHA_256"    }, // Crypto
        { TableToDescriptor(eTable::H_SHA_384),     "Table::H_SHA_384"    }, // Crypto
        { TableToDescriptor(eTable::H_SHA_512),     "Table::H_SHA_512"    }, // Crypto
        { TableToDescriptor(eTable::Log),           "Table::Log"          }, // Log
        { TableToDescriptor(eTable::LogList),       "Table::LogList"      }, // Log
        { TableToDescriptor(eTable::LockingInfo),   "Table::LockingInfo"  }, // Locking
        { TableToDescriptor(eTable::Locking),       "Table::Locking"      }, // Locking
        { TableToDescriptor(eTable::MBRControl),    "Table::MBRControl"   }, // Locking
        { TableToDescriptor(eTable::MBR),           "Table::MBR"          }, // Locking
        { TableToDescriptor(eTable::K_AES_128),     "Table::K_AES_128"    }, // Locking
        { TableToDescriptor(eTable::K_AES_256),     "Table::K_AES_256"    }, // Locking
    };


    constexpr std::initializer_list<std::pair<Uid, std::string_view>> methods = {
        {eMethod::Properties,           "MethodID::Properties"         },
        { eMethod::StartSession,        "MethodID::StartSession"       },
        { eMethod::SyncSession,         "MethodID::SyncSession"        },
        { eMethod::StartTrustedSession, "MethodID::StartTrustedSession"},
        { eMethod::SyncTrustedSession,  "MethodID::SyncTrustedSession" },
        { eMethod::CloseSession,        "MethodID::CloseSession"       },
        { eMethod::DeleteSP,            "MethodID::DeleteSP"           },
        { eMethod::CreateTable,         "MethodID::CreateTable"        },
        { eMethod::Delete,              "MethodID::Delete"             },
        { eMethod::CreateRow,           "MethodID::CreateRow"          },
        { eMethod::DeleteRow,           "MethodID::DeleteRow"          },
        { eMethod::Next,                "MethodID::Next"               },
        { eMethod::GetFreeSpace,        "MethodID::GetFreeSpace"       },
        { eMethod::GetFreeRows,         "MethodID::GetFreeRows"        },
        { eMethod::DeleteMethod,        "MethodID::DeleteMethod"       },
        { eMethod::GetACL,              "MethodID::GetACL"             },
        { eMethod::AddACE,              "MethodID::AddACE"             },
        { eMethod::RemoveACE,           "MethodID::RemoveACE"          },
        { eMethod::GenKey,              "MethodID::GenKey"             },
        { eMethod::GetPackage,          "MethodID::GetPackage"         },
        { eMethod::SetPackage,          "MethodID::SetPackage"         },
        { eMethod::Get,                 "MethodID::Get"                },
        { eMethod::Set,                 "MethodID::Set"                },
        { eMethod::Authenticate,        "MethodID::Authenticate"       },
        { eMethod::Obsolete_0,          "MethodID::Obsolete_0"         },
        { eMethod::Obsolete_1,          "MethodID::Obsolete_1"         },
        { eMethod::Obsolete_2,          "MethodID::Obsolete_2"         },
        { eMethod::IssueSP,             "MethodID::IssueSP"            },
        { eMethod::GetClock,            "MethodID::GetClock"           },
        { eMethod::ResetClock,          "MethodID::ResetClock"         },
        { eMethod::SetClockHigh,        "MethodID::SetClockHigh"       },
        { eMethod::SetLagHigh,          "MethodID::SetLagHigh"         },
        { eMethod::SetClockLow,         "MethodID::SetClockLow"        },
        { eMethod::SetLagLow,           "MethodID::SetLagLow"          },
        { eMethod::IncrementCounter,    "MethodID::IncrementCounter"   },
        { eMethod::Random,              "MethodID::Random"             },
        { eMethod::Salt,                "MethodID::Salt"               },
        { eMethod::DecryptInit,         "MethodID::DecryptInit"        },
        { eMethod::Decrypt,             "MethodID::Decrypt"            },
        { eMethod::DecryptFinalize,     "MethodID::DecryptFinalize"    },
        { eMethod::EncryptInit,         "MethodID::EncryptInit"        },
        { eMethod::Encrypt,             "MethodID::Encrypt"            },
        { eMethod::EncryptFinalize,     "MethodID::EncryptFinalize"    },
        { eMethod::HMACInit,            "MethodID::HMACInit"           },
        { eMethod::HMAC,                "MethodID::HMAC"               },
        { eMethod::HMACFinalize,        "MethodID::HMACFinalize"       },
        { eMethod::HashInit,            "MethodID::HashInit"           },
        { eMethod::Hash,                "MethodID::Hash"               },
        { eMethod::HashFinalize,        "MethodID::HashFinalize"       },
        { eMethod::Sign,                "MethodID::Sign"               },
        { eMethod::Verify,              "MethodID::Verify"             },
        { eMethod::XOR,                 "MethodID::XOR"                },
        { eMethod::AddLog,              "MethodID::AddLog"             },
        { eMethod::CreateLog,           "MethodID::CreateLog"          },
        { eMethod::ClearLog,            "MethodID::ClearLog"           },
        { eMethod::FlushLog,            "MethodID::FlushLog"           },
        { eMethod::Reserved_0,          "MethodID::Reserved_0"         },
        { eMethod::Reserved_1,          "MethodID::Reserved_1"         },
        { eMethod::Reserved_2,          "MethodID::Reserved_2"         },
        { eMethod::Reserved_3,          "MethodID::Reserved_3"         },
    };


    constexpr std::initializer_list<std::pair<Uid, std::string_view>> singleRowTables = {
        {eTableSingleRows::SPInfo,       "SPInfo::SPInfo"          },
        { eTableSingleRows::TPerInfo,    "TPerInfo::TPerInfo"      },
        { eTableSingleRows::LockingInfo, "LockingInfo::LockingInfo"},
        { eTableSingleRows::MBRControl,  "MBRControl::MBRControl"  },
    };


    constexpr std::initializer_list<std::pair<Uid, std::string_view>> authorities = {
        {0x0000'0009'0000'0001,  "Authority::Anybody"  }, // Base
        { 0x0000'0009'0000'0002, "Authority::Admins"   }, // Base
        { 0x0000'0009'0000'0003, "Authority::Makers"   }, // Base
        { 0x0000'0009'0000'0004, "Authority::MakerSymK"}, // Base
        { 0x0000'0009'0000'0005, "Authority::MakerPuK" }, // Base
        { 0x0000'0009'0000'0006, "Authority::SID"      }, // Base
        { 0x0000'0009'0000'0007, "Authority::TPerSign" }, // Base
        { 0x0000'0009'0000'0008, "Authority::TPerExch" }, // Base
        { 0x0000'0009'0000'0009, "Authority::AdminExch"}, // Base
        { 0x0000'0009'0000'0201, "Authority::Issuers"  }, // Admin
        { 0x0000'0009'0000'0202, "Authority::Editors"  }, // Admin
        { 0x0000'0009'0000'0203, "Authority::Deleters" }, // Admin
        { 0x0000'0009'0000'0204, "Authority::Servers"  }, // Admin
        { 0x0000'0009'0000'0205, "Authority::Reserve0" }, // Admin
        { 0x0000'0009'0000'0206, "Authority::Reserve1" }, // Admin
        { 0x0000'0009'0000'0207, "Authority::Reserve2" }, // Admin
        { 0x0000'0009'0000'0208, "Authority::Reserve3" }, // Admin
    };


    constexpr std::initializer_list<std::pair<Uid, std::string_view>> types = {
        {eType::unknown_type,            "Type::unknown_type"          },
        { eType::boolean,                "Type::boolean"               },
        { eType::integer_1,              "Type::integer_1"             },
        { eType::integer_2,              "Type::integer_2"             },
        { eType::uinteger_1,             "Type::uinteger_1"            },
        { eType::uinteger_2,             "Type::uinteger_2"            },
        { eType::uinteger_4,             "Type::uinteger_4"            },
        { eType::uinteger_8,             "Type::uinteger_8"            },
        { eType::max_bytes_32,           "Type::max_bytes_32"          },
        { eType::max_bytes_64,           "Type::max_bytes_64"          },
        { eType::bytes_4,                "Type::bytes_4"               },
        { eType::bytes_12,               "Type::bytes_12"              },
        { eType::bytes_16,               "Type::bytes_16"              },
        { eType::bytes_32,               "Type::bytes_32"              },
        { eType::bytes_64,               "Type::bytes_64"              },
        { eType::key_128,                "Type::key_128"               },
        { eType::key_256,                "Type::key_256"               },
        { eType::type_def,               "Type::type_def"              },
        { eType::name,                   "Type::name"                  },
        { eType::password,               "Type::password"              },
        { eType::year_enum,              "Type::year_enum"             },
        { eType::month_enum,             "Type::month_enum"            },
        { eType::day_enum,               "Type::day_enum"              },
        { eType::Year,                   "Type::Year"                  },
        { eType::Month,                  "Type::Month"                 },
        { eType::Day,                    "Type::Day"                   },
        { eType::date,                   "Type::date"                  },
        { eType::messaging_type,         "Type::messaging_type"        },
        { eType::hash_protocol,          "Type::hash_protocol"         },
        { eType::auth_method,            "Type::auth_method"           },
        { eType::log_select,             "Type::log_select"            },
        { eType::protect_types,          "Type::protect_types"         },
        { eType::reencrypt_request,      "Type::reencrypt_request"     },
        { eType::reencrypt_state,        "Type::reencrypt_state"       },
        { eType::reset_types,            "Type::reset_types"           },
        { eType::adv_key_mode,           "Type::adv_key_mode"          },
        { eType::verify_mode,            "Type::verify_mode"           },
        { eType::last_reenc_stat,        "Type::last_reenc_stat"       },
        { eType::gen_status,             "Type::gen_status"            },
        { eType::enc_supported,          "Type::enc_supported"         },
        { eType::keys_avail_conds,       "Type::keys_avail_conds"      },
        { eType::symmetric_mode_media,   "Type::symmetric_mode_media"  },
        { eType::uid,                    "Type::uid"                   },
        { eType::table_kind,             "Type::table_kind"            },
        { eType::object_ref,             "Type::object_ref"            },
        { eType::table_ref,              "Type::table_ref"             },
        { eType::byte_table_ref,         "Type::byte_table_ref"        },
        { eType::table_or_object_ref,    "Type::table_or_object_ref"   },
        { eType::Authority_object_ref,   "Type::Authority_object_ref"  },
        { eType::Table_object_ref,       "Type::Table_object_ref"      },
        { eType::MethodID_object_ref,    "Type::MethodID_object_ref"   },
        { eType::SPTemplates_object,     "Type::SPTemplates_object"    },
        { eType::Column_object_ref,      "Type::Column_object_ref"     },
        { eType::Template_object_ref,    "Type::Template_object_ref"   },
        { eType::LogList_object_ref,     "Type::LogList_object_ref"    },
        { eType::cred_object_uidref,     "Type::cred_object_uidref"    },
        { eType::mediakey_object_uidref, "Type::mediakey_object_uidref"},
        { eType::boolean_ACE,            "Type::boolean_ACE"           },
        { eType::ACE_expression,         "Type::ACE_expression"        },
        { eType::AC_element,             "Type::AC_element"            },
        { eType::ACE_object_ref,         "Type::ACE_object_ref"        },
        { eType::ACL,                    "Type::ACL"                   },
        { eType::ACE_columns,            "Type::ACE_columns"           },
        { eType::life_cycle_state,       "Type::life_cycle_state"      },
        { eType::SSC,                    "Type::SSC"                   },
    };

    const NameAndUidFinder& GetFinder() {
        static NameAndUidFinder finder({ tables, tablesDescriptors, methods, singleRowTables, authorities, types }, {});
        return finder;
    }

} // namespace core


std::shared_ptr<Module> CoreModule::Get() {
    static const auto instance = std::make_shared<CoreModule>();
    return instance;
}


std::string_view CoreModule::ModuleName() const {
    return "Core";
}


eModuleKind CoreModule::ModuleKind() const {
    return eModuleKind::CORE;
}


std::optional<std::string> CoreModule::FindName(Uid uid, std::optional<Uid>) const {
    return core::GetFinder().Find(uid);
}


std::optional<Uid> CoreModule::FindUid(std::string_view name, std::optional<Uid>) const {
    return core::GetFinder().Find(name);
}


std::optional<TableDesc> CoreModule::FindTable(Uid table) const {
    using namespace core;
    static const auto lut = [] {
        std::unordered_map<Uid, TableDescStatic> lut;
        for (auto& [uid, desc] : TableDescs()) {
            lut.insert({ uid, desc });
        }
        return lut;
    }();
    const auto it = lut.find(table);
    if (it != lut.end()) {
        std::vector<ColumnDesc> columns;
        for (const auto& column : it->second.columns) {
            columns.emplace_back(std::string(column.name), column.isUnique, *FindType(column.type));
        }
        return TableDesc{ std::string(it->second.name), it->second.kind, std::move(columns), it->second.singleRow };
    }
    return std::nullopt;
}


std::optional<Type> CoreModule::FindType(Uid lookupUid) const {
    using namespace core;
    static const auto lut = [] {
        std::unordered_map<Uid, Type> lut;
        for (auto& [uid, def] : core::TypeDefs()) {
            lut.insert_or_assign(uid, def);
        }
        return lut;
    }();
    const auto it = lut.find(lookupUid);
    return it != lut.end() ? std::optional(it->second) : std::nullopt;
}

} // namespace sedmgr