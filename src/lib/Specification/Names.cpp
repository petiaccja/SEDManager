#include "Names.hpp"

#include "Authorities.hpp"
#include "Methods.hpp"
#include "SecurityProviders.hpp"
#include "Tables.hpp"

#include <string>
#include <unordered_map>
#include <utility>



constexpr std::initializer_list<std::pair<Uid, std::string_view>> spNames = {
    {opal::eSecurityProvider::Admin,    "AdminSP"  },
    { opal::eSecurityProvider::Locking, "LockingSP"},
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> authNames = {
    {eAuthority::Anybody,       "Anybody"  },
    { eAuthority::Admins,       "Admins"   },
    { eAuthority::Makers,       "Makers"   },
    { eAuthority::MakerSymK,    "MakerSymK"},
    { eAuthority::MakerPuK,     "MakerPuK" },
    { eAuthority::SID,          "SID"      },
    { eAuthority::TPerSign,     "TPerSign" },
    { eAuthority::TPerExch,     "TPerExch" },
    { eAuthority::AdminExch,    "AdminExch"},
    { eAuthority::Issuers,      "Issuers"  },
    { eAuthority::Editors,      "Editors"  },
    { eAuthority::Deleters,     "Deleters" },
    { eAuthority::Servers,      "Servers"  },
    { opal::eAuthority::PSID,   "PSID"     },
    { opal::eAuthority::Admin1, "Admin1"   },
    { opal::eAuthority::Admin2, "Admin2"   },
    { opal::eAuthority::Admin3, "Admin3"   },
    { opal::eAuthority::Admin4, "Admin4"   },
    { opal::eAuthority::Users,  "Users"    },
    { opal::eAuthority::User1,  "User1"    },
    { opal::eAuthority::User2,  "User2"    },
    { opal::eAuthority::User3,  "User3"    },
    { opal::eAuthority::User4,  "User4"    },
    { opal::eAuthority::User5,  "User5"    },
    { opal::eAuthority::User6,  "User6"    },
    { opal::eAuthority::User7,  "User7"    },
    { opal::eAuthority::User8,  "User8"    },
    { opal::eAuthority::User9,  "User9"    },
    { opal::eAuthority::User10, "User10"   },
    { opal::eAuthority::User11, "User11"   },
    { opal::eAuthority::User12, "User12"   },
};

constexpr std::initializer_list<std::pair<Uid, std::string_view>> methodNames = {
    {eMethod::Properties,           "Properties"         },
    { eMethod::StartSession,        "StartSession"       },
    { eMethod::SyncSession,         "SyncSession"        },
    { eMethod::StartTrustedSession, "StartTrustedSession"},
    { eMethod::SyncTrustedSession,  "SyncTrustedSession" },
    { eMethod::CloseSession,        "CloseSession"       },
    { eMethod::DeleteSP,            "DeleteSP"           },
    { eMethod::CreateTable,         "CreateTable"        },
    { eMethod::Delete,              "Delete"             },
    { eMethod::CreateRow,           "CreateRow"          },
    { eMethod::DeleteRow,           "DeleteRow"          },
    { eMethod::Next,                "Next"               },
    { eMethod::GetFreeSpace,        "GetFreeSpace"       },
    { eMethod::GetFreeRows,         "GetFreeRows"        },
    { eMethod::DeleteMethod,        "DeleteMethod"       },
    { eMethod::GetACL,              "GetACL"             },
    { eMethod::AddACE,              "AddACE"             },
    { eMethod::RemoveACE,           "RemoveACE"          },
    { eMethod::GenKey,              "GenKey"             },
    { eMethod::GetPackage,          "GetPackage"         },
    { eMethod::SetPackage,          "SetPackage"         },
    { eMethod::Get,                 "Get"                },
    { eMethod::Set,                 "Set"                },
    { eMethod::Authenticate,        "Authenticate"       },
    { eMethod::Obsolete_0,          "Obsolete_0"         },
    { eMethod::Obsolete_1,          "Obsolete_1"         },
    { eMethod::Obsolete_2,          "Obsolete_2"         },
    { eMethod::IssueSP,             "IssueSP"            },
    { eMethod::GetClock,            "GetClock"           },
    { eMethod::ResetClock,          "ResetClock"         },
    { eMethod::SetClockHigh,        "SetClockHigh"       },
    { eMethod::SetLagHigh,          "SetLagHigh"         },
    { eMethod::SetClockLow,         "SetClockLow"        },
    { eMethod::SetLagLow,           "SetLagLow"          },
    { eMethod::IncrementCounter,    "IncrementCounter"   },
    { eMethod::Random,              "Random"             },
    { eMethod::Salt,                "Salt"               },
    { eMethod::DecryptInit,         "DecryptInit"        },
    { eMethod::Decrypt,             "Decrypt"            },
    { eMethod::DecryptFinalize,     "DecryptFinalize"    },
    { eMethod::EncryptInit,         "EncryptInit"        },
    { eMethod::Encrypt,             "Encrypt"            },
    { eMethod::EncryptFinalize,     "EncryptFinalize"    },
    { eMethod::HMACInit,            "HMACInit"           },
    { eMethod::HMAC,                "HMAC"               },
    { eMethod::HMACFinalize,        "HMACFinalize"       },
    { eMethod::HashInit,            "HashInit"           },
    { eMethod::Hash,                "Hash"               },
    { eMethod::HashFinalize,        "HashFinalize"       },
    { eMethod::Sign,                "Sign"               },
    { eMethod::Verify,              "Verify"             },
    { eMethod::XOR,                 "XOR"                },
    { eMethod::AddLog,              "AddLog"             },
    { eMethod::CreateLog,           "CreateLog"          },
    { eMethod::ClearLog,            "ClearLog"           },
    { eMethod::FlushLog,            "FlushLog"           },
    { eMethod::Reserved_0,          "Reserved_0"         },
    { eMethod::Reserved_1,          "Reserved_1"         },
    { eMethod::Reserved_2,          "Reserved_2"         },
    { eMethod::Reserved_3,          "Reserved_3"         },
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> tableNames = {
    {eTable::Table,          "Table"        },
    { eTable::SPInfo,        "SPInfo"       },
    { eTable::SPTemplates,   "SPTemplates"  },
    { eTable::Column,        "Column"       },
    { eTable::Type,          "Type"         },
    { eTable::MethodID,      "MethodID"     },
    { eTable::AccessControl, "AccessControl"},
    { eTable::ACE,           "ACE"          },
    { eTable::Authority,     "Authority"    },
    { eTable::Certificates,  "Certificates" },
    { eTable::C_PIN,         "C_PIN"        },
    { eTable::C_RSA_1024,    "C_RSA_1024"   },
    { eTable::C_RSA_2048,    "C_RSA_2048"   },
    { eTable::C_AES_128,     "C_AES_128"    },
    { eTable::C_AES_256,     "C_AES_256"    },
    { eTable::C_EC_160,      "C_EC_160"     },
    { eTable::C_EC_192,      "C_EC_192"     },
    { eTable::C_EC_224,      "C_EC_224"     },
    { eTable::C_EC_256,      "C_EC_256"     },
    { eTable::C_EC_384,      "C_EC_384"     },
    { eTable::C_EC_521,      "C_EC_521"     },
    { eTable::C_EC_163,      "C_EC_163"     },
    { eTable::C_EC_233,      "C_EC_233"     },
    { eTable::C_EC_283,      "C_EC_283"     },
    { eTable::C_HMAC_160,    "C_HMAC_160"   },
    { eTable::C_HMAC_256,    "C_HMAC_256"   },
    { eTable::C_HMAC_384,    "C_HMAC_384"   },
    { eTable::C_HMAC_512,    "C_HMAC_512"   },
    { eTable::SecretProtect, "SecretProtect"},
    { eTable::TPerInfo,      "TPerInfo"     },
    { eTable::CryptoSuite,   "CryptoSuite"  },
    { eTable::Template,      "Template"     },
    { eTable::SP,            "SP"           },
    { eTable::ClockTime,     "ClockTime"    },
    { eTable::H_SHA_1,       "H_SHA_1"      },
    { eTable::H_SHA_256,     "H_SHA_256"    },
    { eTable::H_SHA_384,     "H_SHA_384"    },
    { eTable::H_SHA_512,     "H_SHA_512"    },
    { eTable::Log,           "Log"          },
    { eTable::LogList,       "LogList"      },
    { eTable::LockingInfo,   "LockingInfo"  },
    { eTable::Locking,       "Locking"      },
    { eTable::MBRControl,    "MBRControl"   },
    { eTable::MBR,           "MBR"          },
    { eTable::K_AES_128,     "K_AES_128"    },
    { eTable::K_AES_256,     "K_AES_256"    },
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> tableDescriptorNames = {
    {TableToDescriptor(eTable::Table),          "Table::Table"        },
    { TableToDescriptor(eTable::SPInfo),        "Table::SPInfo"       },
    { TableToDescriptor(eTable::SPTemplates),   "Table::SPTemplates"  },
    { TableToDescriptor(eTable::Column),        "Table::Column"       },
    { TableToDescriptor(eTable::Type),          "Table::Type"         },
    { TableToDescriptor(eTable::MethodID),      "Table::MethodID"     },
    { TableToDescriptor(eTable::AccessControl), "Table::AccessControl"},
    { TableToDescriptor(eTable::ACE),           "Table::ACE"          },
    { TableToDescriptor(eTable::Authority),     "Table::Authority"    },
    { TableToDescriptor(eTable::Certificates),  "Table::Certificates" },
    { TableToDescriptor(eTable::C_PIN),         "Table::C_PIN"        },
    { TableToDescriptor(eTable::C_RSA_1024),    "Table::C_RSA_1024"   },
    { TableToDescriptor(eTable::C_RSA_2048),    "Table::C_RSA_2048"   },
    { TableToDescriptor(eTable::C_AES_128),     "Table::C_AES_128"    },
    { TableToDescriptor(eTable::C_AES_256),     "Table::C_AES_256"    },
    { TableToDescriptor(eTable::C_EC_160),      "Table::C_EC_160"     },
    { TableToDescriptor(eTable::C_EC_192),      "Table::C_EC_192"     },
    { TableToDescriptor(eTable::C_EC_224),      "Table::C_EC_224"     },
    { TableToDescriptor(eTable::C_EC_256),      "Table::C_EC_256"     },
    { TableToDescriptor(eTable::C_EC_384),      "Table::C_EC_384"     },
    { TableToDescriptor(eTable::C_EC_521),      "Table::C_EC_521"     },
    { TableToDescriptor(eTable::C_EC_163),      "Table::C_EC_163"     },
    { TableToDescriptor(eTable::C_EC_233),      "Table::C_EC_233"     },
    { TableToDescriptor(eTable::C_EC_283),      "Table::C_EC_283"     },
    { TableToDescriptor(eTable::C_HMAC_160),    "Table::C_HMAC_160"   },
    { TableToDescriptor(eTable::C_HMAC_256),    "Table::C_HMAC_256"   },
    { TableToDescriptor(eTable::C_HMAC_384),    "Table::C_HMAC_384"   },
    { TableToDescriptor(eTable::C_HMAC_512),    "Table::C_HMAC_512"   },
    { TableToDescriptor(eTable::SecretProtect), "Table::SecretProtect"},
    { TableToDescriptor(eTable::TPerInfo),      "Table::TPerInfo"     },
    { TableToDescriptor(eTable::CryptoSuite),   "Table::CryptoSuite"  },
    { TableToDescriptor(eTable::Template),      "Table::Template"     },
    { TableToDescriptor(eTable::SP),            "Table::SP"           },
    { TableToDescriptor(eTable::ClockTime),     "Table::ClockTime"    },
    { TableToDescriptor(eTable::H_SHA_1),       "Table::H_SHA_1"      },
    { TableToDescriptor(eTable::H_SHA_256),     "Table::H_SHA_256"    },
    { TableToDescriptor(eTable::H_SHA_384),     "Table::H_SHA_384"    },
    { TableToDescriptor(eTable::H_SHA_512),     "Table::H_SHA_512"    },
    { TableToDescriptor(eTable::Log),           "Table::Log"          },
    { TableToDescriptor(eTable::LogList),       "Table::LogList"      },
    { TableToDescriptor(eTable::LockingInfo),   "Table::LockingInfo"  },
    { TableToDescriptor(eTable::Locking),       "Table::Locking"      },
    { TableToDescriptor(eTable::MBRControl),    "Table::MBRControl"   },
    { TableToDescriptor(eTable::MBR),           "Table::MBR"          },
    { TableToDescriptor(eTable::K_AES_128),     "Table::K_AES_128"    },
    { TableToDescriptor(eTable::K_AES_256),     "Table::K_AES_256"    },
};


std::optional<std::string_view> GetName(Uid uid) {
    auto amend = [](auto& mapping, auto pairs) {
        for (const auto& [uid, name] : pairs) {
            mapping.insert({ uint64_t(uid), name });
        }
    };
    static const auto mapping = [&] {
        std::unordered_map<uint64_t, std::string_view> mapping;
        amend(mapping, spNames);
        amend(mapping, authNames);
        amend(mapping, methodNames);
        amend(mapping, tableNames);
        amend(mapping, tableDescriptorNames);
        return mapping;
    }();
    const auto it = mapping.find(uint64_t(uid));
    if (it != mapping.end()) {
        return it->second;
    }
    return std::nullopt;
}


std::string GetNameOrUid(Uid uid) {
    std::string_view properName = GetName(uid).value_or(std::string_view{});
    return !properName.empty() ? std::string(properName) : std::format("{:#018x}", uint64_t(uid));
}


std::optional<Uid> GetUid(std::string_view name) {
    auto amend = [](auto& mapping, auto pairs) {
        for (const auto& [uid, name] : pairs) {
            const auto [it, unique] = mapping.insert({ name, uint64_t(uid) });
            if (!unique) {
                throw std::logic_error(std::format("non-unique entry among names: {}", name));
            }
        }
    };
    static const auto mapping = [&] {
        std::unordered_map<std::string_view, Uid> mapping;
        amend(mapping, spNames);
        amend(mapping, authNames);
        amend(mapping, methodNames);
        amend(mapping, tableNames);
        amend(mapping, tableDescriptorNames);
        return mapping;
    }();
    const auto it = mapping.find(name);
    if (it != mapping.end()) {
        return it->second;
    }
    return std::nullopt;
}


std::optional<Uid> GetUidOrHex(std::string_view name) {
    const auto maybeUid = GetUid(name);
    if (maybeUid) {
        return *maybeUid;
    }
    try {
        const uint64_t value = std::bit_cast<uint64_t>(std::stoll(std::string(name), nullptr, 16));
        return Uid(value);
    }
    catch (std::exception& ex) {
        return std::nullopt;
    }
}