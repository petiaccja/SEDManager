#include "CoreModule.hpp"

#include "../Common/Utility.hpp"
#include "Defs/TableDescs.hpp"
#include "Defs/Types.hpp"
#include "Defs/UIDs.hpp"

#include <algorithm>


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


const NameAndUidFinder& GetFinder() {
    static NameAndUidFinder finder({ tables, tablesDescriptors, methods, singleRowTables, authorities }, {});
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
        for (auto& desc : tableDescs) {
            lut.insert({ desc.uid, desc });
        }
        return lut;
    }();
    const auto it = lut.find(table);
    return it != lut.end() ? std::optional(TableDesc(it->second)) : std::nullopt;
}


std::optional<Type> CoreModule::FindType(Uid lookupUid) const {
    using namespace core;
    static const std::unordered_map<Uid, Type> lut = {
        {type_uid(unknown_type),            unknown_type          },
        { type_uid(boolean),                boolean               },
        { type_uid(integer_1),              integer_1             },
        { type_uid(integer_2),              integer_2             },
        { type_uid(uinteger_1),             uinteger_1            },
        { type_uid(uinteger_2),             uinteger_2            },
        { type_uid(uinteger_4),             uinteger_4            },
        { type_uid(uinteger_8),             uinteger_8            },
        { type_uid(max_bytes_32),           max_bytes_32          },
        { type_uid(max_bytes_64),           max_bytes_64          },
        { type_uid(bytes_12),               bytes_12              },
        { type_uid(name),                   name                  },
        { type_uid(password),               password              },
        { type_uid(year_enum),              year_enum             },
        { type_uid(month_enum),             month_enum            },
        { type_uid(day_enum),               day_enum              },
        { type_uid(Year),                   Year                  },
        { type_uid(Month),                  Month                 },
        { type_uid(Day),                    Day                   },
        { type_uid(date),                   date                  },
        { type_uid(messaging_type),         messaging_type        },
        { type_uid(hash_protocol),          hash_protocol         },
        { type_uid(auth_method),            auth_method           },
        { type_uid(log_select),             log_select            },
        { type_uid(protect_types),          protect_types         },
        { type_uid(reencrypt_request),      reencrypt_request     },
        { type_uid(reencrypt_state),        reencrypt_state       },
        { type_uid(reset_types),            reset_types           },
        { type_uid(adv_key_mode),           adv_key_mode          },
        { type_uid(verify_mode),            verify_mode           },
        { type_uid(last_reenc_stat),        last_reenc_stat       },
        { type_uid(gen_status),             gen_status            },
        { type_uid(enc_supported),          enc_supported         },
        { type_uid(keys_avail_conds),       keys_avail_conds      },
        { type_uid(uid),                    uid                   },
        { type_uid(table_kind),             table_kind            },
        { type_uid(object_ref),             object_ref            },
        { type_uid(table_ref),              table_ref             },
        { type_uid(byte_table_ref),         byte_table_ref        },
        { type_uid(table_or_object_ref),    table_or_object_ref   },
        { type_uid(Authority_object_ref),   Authority_object_ref  },
        { type_uid(Table_object_ref),       Table_object_ref      },
        { type_uid(MethodID_object_ref),    MethodID_object_ref   },
        { type_uid(SPTemplates_object),     SPTemplates_object    },
        { type_uid(Column_object_ref),      Column_object_ref     },
        { type_uid(Template_object_ref),    Template_object_ref   },
        { type_uid(LogList_object_ref),     LogList_object_ref    },
        { type_uid(cred_object_uidref),     cred_object_uidref    },
        { type_uid(mediakey_object_uidref), mediakey_object_uidref},
        { type_uid(boolean_ACE),            boolean_ACE           },
        { type_uid(ACE_expression),         ACE_expression        },
        { type_uid(AC_element),             AC_element            },
        { type_uid(ACE_object_ref),         ACE_object_ref        },
        { type_uid(ACL),                    ACL                   },
        { type_uid(ACE_columns),            ACE_columns           },
        { type_uid(life_cycle_state),       life_cycle_state      },
        { type_uid(SSC),                    SSC                   },
    };
    const auto it = lut.find(lookupUid);
    return it != lut.end() ? std::optional(it->second) : std::nullopt;
}