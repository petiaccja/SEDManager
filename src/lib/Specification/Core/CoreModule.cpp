#include "CoreModule.hpp"

#include "../Common/Utility.hpp"
#include "Defs/TableDescs.hpp"
#include "Defs/TypeDefs.hpp"
#include "Defs/UIDs.hpp"

#include <algorithm>


namespace sedmgr {

namespace core {

    constexpr std::initializer_list<std::pair<UID, std::string_view>> tables = {
        {UID(eTable::Table),          "Table"        }, // Base
        { UID(eTable::SPInfo),        "SPInfo"       }, // Base
        { UID(eTable::SPTemplates),   "SPTemplates"  }, // Base
        { UID(eTable::Column),        "Column"       }, // Base
        { UID(eTable::Type),          "Type"         }, // Base
        { UID(eTable::MethodID),      "MethodID"     }, // Base
        { UID(eTable::AccessControl), "AccessControl"}, // Base
        { UID(eTable::ACE),           "ACE"          }, // Base
        { UID(eTable::Authority),     "Authority"    }, // Base
        { UID(eTable::Certificates),  "Certificates" }, // Base
        { UID(eTable::C_PIN),         "C_PIN"        }, // Base
        { UID(eTable::C_RSA_1024),    "C_RSA_1024"   }, // Base
        { UID(eTable::C_RSA_2048),    "C_RSA_2048"   }, // Base
        { UID(eTable::C_AES_128),     "C_AES_128"    }, // Base
        { UID(eTable::C_AES_256),     "C_AES_256"    }, // Base
        { UID(eTable::C_EC_160),      "C_EC_160"     }, // Base
        { UID(eTable::C_EC_192),      "C_EC_192"     }, // Base
        { UID(eTable::C_EC_224),      "C_EC_224"     }, // Base
        { UID(eTable::C_EC_256),      "C_EC_256"     }, // Base
        { UID(eTable::C_EC_384),      "C_EC_384"     }, // Base
        { UID(eTable::C_EC_521),      "C_EC_521"     }, // Base
        { UID(eTable::C_EC_163),      "C_EC_163"     }, // Base
        { UID(eTable::C_EC_233),      "C_EC_233"     }, // Base
        { UID(eTable::C_EC_283),      "C_EC_283"     }, // Base
        { UID(eTable::C_HMAC_160),    "C_HMAC_160"   }, // Base
        { UID(eTable::C_HMAC_256),    "C_HMAC_256"   }, // Base
        { UID(eTable::C_HMAC_384),    "C_HMAC_384"   }, // Base
        { UID(eTable::C_HMAC_512),    "C_HMAC_512"   }, // Base
        { UID(eTable::SecretProtect), "SecretProtect"}, // Base
        { UID(eTable::TPerInfo),      "TPerInfo"     }, // Admin
        { UID(eTable::CryptoSuite),   "CryptoSuite"  }, // Admin
        { UID(eTable::Template),      "Template"     }, // Admin
        { UID(eTable::SP),            "SP"           }, // Admin
        { UID(eTable::ClockTime),     "ClockTime"    }, // Clock
        { UID(eTable::H_SHA_1),       "H_SHA_1"      }, // Crypto
        { UID(eTable::H_SHA_256),     "H_SHA_256"    }, // Crypto
        { UID(eTable::H_SHA_384),     "H_SHA_384"    }, // Crypto
        { UID(eTable::H_SHA_512),     "H_SHA_512"    }, // Crypto
        { UID(eTable::Log),           "Log"          }, // Log
        { UID(eTable::LogList),       "LogList"      }, // Log
        { UID(eTable::LockingInfo),   "LockingInfo"  }, // Locking
        { UID(eTable::Locking),       "Locking"      }, // Locking
        { UID(eTable::MBRControl),    "MBRControl"   }, // Locking
        { UID(eTable::MBR),           "MBR"          }, // Locking
        { UID(eTable::K_AES_128),     "K_AES_128"    }, // Locking
        { UID(eTable::K_AES_256),     "K_AES_256"    }, // Locking
    };


    constexpr std::initializer_list<std::pair<UID, std::string_view>> tablesDescriptors = {
        {UID(eTable::Table).ToDescriptor(),          "Table::Table"        }, // Base
        { UID(eTable::SPInfo).ToDescriptor(),        "Table::SPInfo"       }, // Base
        { UID(eTable::SPTemplates).ToDescriptor(),   "Table::SPTemplates"  }, // Base
        { UID(eTable::Column).ToDescriptor(),        "Table::Column"       }, // Base
        { UID(eTable::Type).ToDescriptor(),          "Table::Type"         }, // Base
        { UID(eTable::MethodID).ToDescriptor(),      "Table::MethodID"     }, // Base
        { UID(eTable::AccessControl).ToDescriptor(), "Table::AccessControl"}, // Base
        { UID(eTable::ACE).ToDescriptor(),           "Table::ACE"          }, // Base
        { UID(eTable::Authority).ToDescriptor(),     "Table::Authority"    }, // Base
        { UID(eTable::Certificates).ToDescriptor(),  "Table::Certificates" }, // Base
        { UID(eTable::C_PIN).ToDescriptor(),         "Table::C_PIN"        }, // Base
        { UID(eTable::C_RSA_1024).ToDescriptor(),    "Table::C_RSA_1024"   }, // Base
        { UID(eTable::C_RSA_2048).ToDescriptor(),    "Table::C_RSA_2048"   }, // Base
        { UID(eTable::C_AES_128).ToDescriptor(),     "Table::C_AES_128"    }, // Base
        { UID(eTable::C_AES_256).ToDescriptor(),     "Table::C_AES_256"    }, // Base
        { UID(eTable::C_EC_160).ToDescriptor(),      "Table::C_EC_160"     }, // Base
        { UID(eTable::C_EC_192).ToDescriptor(),      "Table::C_EC_192"     }, // Base
        { UID(eTable::C_EC_224).ToDescriptor(),      "Table::C_EC_224"     }, // Base
        { UID(eTable::C_EC_256).ToDescriptor(),      "Table::C_EC_256"     }, // Base
        { UID(eTable::C_EC_384).ToDescriptor(),      "Table::C_EC_384"     }, // Base
        { UID(eTable::C_EC_521).ToDescriptor(),      "Table::C_EC_521"     }, // Base
        { UID(eTable::C_EC_163).ToDescriptor(),      "Table::C_EC_163"     }, // Base
        { UID(eTable::C_EC_233).ToDescriptor(),      "Table::C_EC_233"     }, // Base
        { UID(eTable::C_EC_283).ToDescriptor(),      "Table::C_EC_283"     }, // Base
        { UID(eTable::C_HMAC_160).ToDescriptor(),    "Table::C_HMAC_160"   }, // Base
        { UID(eTable::C_HMAC_256).ToDescriptor(),    "Table::C_HMAC_256"   }, // Base
        { UID(eTable::C_HMAC_384).ToDescriptor(),    "Table::C_HMAC_384"   }, // Base
        { UID(eTable::C_HMAC_512).ToDescriptor(),    "Table::C_HMAC_512"   }, // Base
        { UID(eTable::SecretProtect).ToDescriptor(), "Table::SecretProtect"}, // Base
        { UID(eTable::TPerInfo).ToDescriptor(),      "Table::TPerInfo"     }, // Admin
        { UID(eTable::CryptoSuite).ToDescriptor(),   "Table::CryptoSuite"  }, // Admin
        { UID(eTable::Template).ToDescriptor(),      "Table::Template"     }, // Admin
        { UID(eTable::SP).ToDescriptor(),            "Table::SP"           }, // Admin
        { UID(eTable::ClockTime).ToDescriptor(),     "Table::ClockTime"    }, // Clock
        { UID(eTable::H_SHA_1).ToDescriptor(),       "Table::H_SHA_1"      }, // Crypto
        { UID(eTable::H_SHA_256).ToDescriptor(),     "Table::H_SHA_256"    }, // Crypto
        { UID(eTable::H_SHA_384).ToDescriptor(),     "Table::H_SHA_384"    }, // Crypto
        { UID(eTable::H_SHA_512).ToDescriptor(),     "Table::H_SHA_512"    }, // Crypto
        { UID(eTable::Log).ToDescriptor(),           "Table::Log"          }, // Log
        { UID(eTable::LogList).ToDescriptor(),       "Table::LogList"      }, // Log
        { UID(eTable::LockingInfo).ToDescriptor(),   "Table::LockingInfo"  }, // Locking
        { UID(eTable::Locking).ToDescriptor(),       "Table::Locking"      }, // Locking
        { UID(eTable::MBRControl).ToDescriptor(),    "Table::MBRControl"   }, // Locking
        { UID(eTable::MBR).ToDescriptor(),           "Table::MBR"          }, // Locking
        { UID(eTable::K_AES_128).ToDescriptor(),     "Table::K_AES_128"    }, // Locking
        { UID(eTable::K_AES_256).ToDescriptor(),     "Table::K_AES_256"    }, // Locking
    };


    constexpr std::initializer_list<std::pair<UID, std::string_view>> methods = {
        {UID(eMethod::Properties),           "MethodID::Properties"         },
        { UID(eMethod::StartSession),        "MethodID::StartSession"       },
        { UID(eMethod::SyncSession),         "MethodID::SyncSession"        },
        { UID(eMethod::StartTrustedSession), "MethodID::StartTrustedSession"},
        { UID(eMethod::SyncTrustedSession),  "MethodID::SyncTrustedSession" },
        { UID(eMethod::CloseSession),        "MethodID::CloseSession"       },
        { UID(eMethod::DeleteSP),            "MethodID::DeleteSP"           },
        { UID(eMethod::CreateTable),         "MethodID::CreateTable"        },
        { UID(eMethod::Delete),              "MethodID::Delete"             },
        { UID(eMethod::CreateRow),           "MethodID::CreateRow"          },
        { UID(eMethod::DeleteRow),           "MethodID::DeleteRow"          },
        { UID(eMethod::Next),                "MethodID::Next"               },
        { UID(eMethod::GetFreeSpace),        "MethodID::GetFreeSpace"       },
        { UID(eMethod::GetFreeRows),         "MethodID::GetFreeRows"        },
        { UID(eMethod::DeleteMethod),        "MethodID::DeleteMethod"       },
        { UID(eMethod::GetACL),              "MethodID::GetACL"             },
        { UID(eMethod::AddACE),              "MethodID::AddACE"             },
        { UID(eMethod::RemoveACE),           "MethodID::RemoveACE"          },
        { UID(eMethod::GenKey),              "MethodID::GenKey"             },
        { UID(eMethod::GetPackage),          "MethodID::GetPackage"         },
        { UID(eMethod::SetPackage),          "MethodID::SetPackage"         },
        { UID(eMethod::Get),                 "MethodID::Get"                },
        { UID(eMethod::Set),                 "MethodID::Set"                },
        { UID(eMethod::Authenticate),        "MethodID::Authenticate"       },
        { UID(eMethod::Obsolete_0),          "MethodID::Obsolete_0"         },
        { UID(eMethod::Obsolete_1),          "MethodID::Obsolete_1"         },
        { UID(eMethod::Obsolete_2),          "MethodID::Obsolete_2"         },
        { UID(eMethod::IssueSP),             "MethodID::IssueSP"            },
        { UID(eMethod::GetClock),            "MethodID::GetClock"           },
        { UID(eMethod::ResetClock),          "MethodID::ResetClock"         },
        { UID(eMethod::SetClockHigh),        "MethodID::SetClockHigh"       },
        { UID(eMethod::SetLagHigh),          "MethodID::SetLagHigh"         },
        { UID(eMethod::SetClockLow),         "MethodID::SetClockLow"        },
        { UID(eMethod::SetLagLow),           "MethodID::SetLagLow"          },
        { UID(eMethod::IncrementCounter),    "MethodID::IncrementCounter"   },
        { UID(eMethod::Random),              "MethodID::Random"             },
        { UID(eMethod::Salt),                "MethodID::Salt"               },
        { UID(eMethod::DecryptInit),         "MethodID::DecryptInit"        },
        { UID(eMethod::Decrypt),             "MethodID::Decrypt"            },
        { UID(eMethod::DecryptFinalize),     "MethodID::DecryptFinalize"    },
        { UID(eMethod::EncryptInit),         "MethodID::EncryptInit"        },
        { UID(eMethod::Encrypt),             "MethodID::Encrypt"            },
        { UID(eMethod::EncryptFinalize),     "MethodID::EncryptFinalize"    },
        { UID(eMethod::HMACInit),            "MethodID::HMACInit"           },
        { UID(eMethod::HMAC),                "MethodID::HMAC"               },
        { UID(eMethod::HMACFinalize),        "MethodID::HMACFinalize"       },
        { UID(eMethod::HashInit),            "MethodID::HashInit"           },
        { UID(eMethod::Hash),                "MethodID::Hash"               },
        { UID(eMethod::HashFinalize),        "MethodID::HashFinalize"       },
        { UID(eMethod::Sign),                "MethodID::Sign"               },
        { UID(eMethod::Verify),              "MethodID::Verify"             },
        { UID(eMethod::XOR),                 "MethodID::XOR"                },
        { UID(eMethod::AddLog),              "MethodID::AddLog"             },
        { UID(eMethod::CreateLog),           "MethodID::CreateLog"          },
        { UID(eMethod::ClearLog),            "MethodID::ClearLog"           },
        { UID(eMethod::FlushLog),            "MethodID::FlushLog"           },
        { UID(eMethod::Reserved_0),          "MethodID::Reserved_0"         },
        { UID(eMethod::Reserved_1),          "MethodID::Reserved_1"         },
        { UID(eMethod::Reserved_2),          "MethodID::Reserved_2"         },
        { UID(eMethod::Reserved_3),          "MethodID::Reserved_3"         },
    };


    constexpr std::initializer_list<std::pair<UID, std::string_view>> singleRowTables = {
        {UID(eTableSingleRows::SPInfo),       "SPInfo::SPInfo"          },
        { UID(eTableSingleRows::TPerInfo),    "TPerInfo::TPerInfo"      },
        { UID(eTableSingleRows::LockingInfo), "LockingInfo::LockingInfo"},
        { UID(eTableSingleRows::MBRControl),  "MBRControl::MBRControl"  },
    };


    constexpr std::initializer_list<std::pair<UID, std::string_view>> authorities = {
        {0x0000'0009'0000'0001_uid,  "Authority::Anybody"  }, // Base
        { 0x0000'0009'0000'0002_uid, "Authority::Admins"   }, // Base
        { 0x0000'0009'0000'0003_uid, "Authority::Makers"   }, // Base
        { 0x0000'0009'0000'0004_uid, "Authority::MakerSymK"}, // Base
        { 0x0000'0009'0000'0005_uid, "Authority::MakerPuK" }, // Base
        { 0x0000'0009'0000'0006_uid, "Authority::SID"      }, // Base
        { 0x0000'0009'0000'0007_uid, "Authority::TPerSign" }, // Base
        { 0x0000'0009'0000'0008_uid, "Authority::TPerExch" }, // Base
        { 0x0000'0009'0000'0009_uid, "Authority::AdminExch"}, // Base
        { 0x0000'0009'0000'0201_uid, "Authority::Issuers"  }, // Admin
        { 0x0000'0009'0000'0202_uid, "Authority::Editors"  }, // Admin
        { 0x0000'0009'0000'0203_uid, "Authority::Deleters" }, // Admin
        { 0x0000'0009'0000'0204_uid, "Authority::Servers"  }, // Admin
        { 0x0000'0009'0000'0205_uid, "Authority::Reserve0" }, // Admin
        { 0x0000'0009'0000'0206_uid, "Authority::Reserve1" }, // Admin
        { 0x0000'0009'0000'0207_uid, "Authority::Reserve2" }, // Admin
        { 0x0000'0009'0000'0208_uid, "Authority::Reserve3" }, // Admin
    };


    constexpr std::initializer_list<std::pair<UID, std::string_view>> types = {
        {UID(eType::unknown_type),            "Type::unknown_type"          },
        { UID(eType::boolean),                "Type::boolean"               },
        { UID(eType::integer_1),              "Type::integer_1"             },
        { UID(eType::integer_2),              "Type::integer_2"             },
        { UID(eType::uinteger_1),             "Type::uinteger_1"            },
        { UID(eType::uinteger_2),             "Type::uinteger_2"            },
        { UID(eType::uinteger_4),             "Type::uinteger_4"            },
        { UID(eType::uinteger_8),             "Type::uinteger_8"            },
        { UID(eType::max_bytes_32),           "Type::max_bytes_32"          },
        { UID(eType::max_bytes_64),           "Type::max_bytes_64"          },
        { UID(eType::bytes_4),                "Type::bytes_4"               },
        { UID(eType::bytes_12),               "Type::bytes_12"              },
        { UID(eType::bytes_16),               "Type::bytes_16"              },
        { UID(eType::bytes_32),               "Type::bytes_32"              },
        { UID(eType::bytes_64),               "Type::bytes_64"              },
        { UID(eType::key_128),                "Type::key_128"               },
        { UID(eType::key_256),                "Type::key_256"               },
        { UID(eType::type_def),               "Type::type_def"              },
        { UID(eType::name),                   "Type::name"                  },
        { UID(eType::password),               "Type::password"              },
        { UID(eType::year_enum),              "Type::year_enum"             },
        { UID(eType::month_enum),             "Type::month_enum"            },
        { UID(eType::day_enum),               "Type::day_enum"              },
        { UID(eType::Year),                   "Type::Year"                  },
        { UID(eType::Month),                  "Type::Month"                 },
        { UID(eType::Day),                    "Type::Day"                   },
        { UID(eType::date),                   "Type::date"                  },
        { UID(eType::messaging_type),         "Type::messaging_type"        },
        { UID(eType::hash_protocol),          "Type::hash_protocol"         },
        { UID(eType::auth_method),            "Type::auth_method"           },
        { UID(eType::log_select),             "Type::log_select"            },
        { UID(eType::protect_types),          "Type::protect_types"         },
        { UID(eType::reencrypt_request),      "Type::reencrypt_request"     },
        { UID(eType::reencrypt_state),        "Type::reencrypt_state"       },
        { UID(eType::reset_types),            "Type::reset_types"           },
        { UID(eType::adv_key_mode),           "Type::adv_key_mode"          },
        { UID(eType::verify_mode),            "Type::verify_mode"           },
        { UID(eType::last_reenc_stat),        "Type::last_reenc_stat"       },
        { UID(eType::gen_status),             "Type::gen_status"            },
        { UID(eType::enc_supported),          "Type::enc_supported"         },
        { UID(eType::keys_avail_conds),       "Type::keys_avail_conds"      },
        { UID(eType::symmetric_mode_media),   "Type::symmetric_mode_media"  },
        { UID(eType::uid),                    "Type::uid"                   },
        { UID(eType::table_kind),             "Type::table_kind"            },
        { UID(eType::object_ref),             "Type::object_ref"            },
        { UID(eType::table_ref),              "Type::table_ref"             },
        { UID(eType::byte_table_ref),         "Type::byte_table_ref"        },
        { UID(eType::table_or_object_ref),    "Type::table_or_object_ref"   },
        { UID(eType::Authority_object_ref),   "Type::Authority_object_ref"  },
        { UID(eType::Table_object_ref),       "Type::Table_object_ref"      },
        { UID(eType::MethodID_object_ref),    "Type::MethodID_object_ref"   },
        { UID(eType::SPTemplates_object),     "Type::SPTemplates_object"    },
        { UID(eType::Column_object_ref),      "Type::Column_object_ref"     },
        { UID(eType::Template_object_ref),    "Type::Template_object_ref"   },
        { UID(eType::LogList_object_ref),     "Type::LogList_object_ref"    },
        { UID(eType::cred_object_uidref),     "Type::cred_object_uidref"    },
        { UID(eType::mediakey_object_uidref), "Type::mediakey_object_uidref"},
        { UID(eType::boolean_ACE),            "Type::boolean_ACE"           },
        { UID(eType::ACE_expression),         "Type::ACE_expression"        },
        { UID(eType::AC_element),             "Type::AC_element"            },
        { UID(eType::ACE_object_ref),         "Type::ACE_object_ref"        },
        { UID(eType::ACL),                    "Type::ACL"                   },
        { UID(eType::ACE_columns),            "Type::ACE_columns"           },
        { UID(eType::life_cycle_state),       "Type::life_cycle_state"      },
        { UID(eType::SSC),                    "Type::SSC"                   },
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


std::optional<std::string> CoreModule::FindName(UID uid, std::optional<UID>) const {
    return core::GetFinder().Find(uid);
}


std::optional<UID> CoreModule::FindUid(std::string_view name, std::optional<UID>) const {
    return core::GetFinder().Find(name);
}


std::optional<TableDesc> CoreModule::FindTable(UID table) const {
    using namespace core;
    static const auto lut = [] {
        std::unordered_map<UID, TableDescStatic> lut;
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


std::optional<Type> CoreModule::FindType(UID lookupUid) const {
    using namespace core;
    static const auto lut = [] {
        std::unordered_map<UID, Type> lut;
        for (auto& [uid, def] : core::TypeDefs()) {
            lut.insert_or_assign(uid, def);
        }
        return lut;
    }();
    const auto it = lut.find(lookupUid);
    return it != lut.end() ? std::optional(it->second) : std::nullopt;
}

} // namespace sedmgr