#include "Names.hpp"

#include "Identifiers.hpp"
#include "Tables.hpp"

#include <string>
#include <unordered_map>
#include <utility>



constexpr std::initializer_list<std::pair<Uid, std::string_view>> spNames = {
    {opal::eSecurityProvider::Admin,    "SP::Admin"  },
    { opal::eSecurityProvider::Locking, "SP::Locking"},
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> authNames = {
    {eAuthority::Anybody,       "Authority::Anybody"  },
    { eAuthority::Admins,       "Authority::Admins"   },
    { eAuthority::Makers,       "Authority::Makers"   },
    { eAuthority::MakerSymK,    "Authority::MakerSymK"},
    { eAuthority::MakerPuK,     "Authority::MakerPuK" },
    { eAuthority::SID,          "Authority::SID"      },
    { eAuthority::TPerSign,     "Authority::TPerSign" },
    { eAuthority::TPerExch,     "Authority::TPerExch" },
    { eAuthority::AdminExch,    "Authority::AdminExch"},
    { eAuthority::Issuers,      "Authority::Issuers"  },
    { eAuthority::Editors,      "Authority::Editors"  },
    { eAuthority::Deleters,     "Authority::Deleters" },
    { eAuthority::Servers,      "Authority::Servers"  },
    { opal::eAuthority::PSID,   "Authority::PSID"     },
    { opal::eAuthority::Admin1, "Authority::Admin1"   },
    { opal::eAuthority::Admin2, "Authority::Admin2"   },
    { opal::eAuthority::Admin3, "Authority::Admin3"   },
    { opal::eAuthority::Admin4, "Authority::Admin4"   },
    { opal::eAuthority::Users,  "Authority::Users"    },
    { opal::eAuthority::User1,  "Authority::User1"    },
    { opal::eAuthority::User2,  "Authority::User2"    },
    { opal::eAuthority::User3,  "Authority::User3"    },
    { opal::eAuthority::User4,  "Authority::User4"    },
    { opal::eAuthority::User5,  "Authority::User5"    },
    { opal::eAuthority::User6,  "Authority::User6"    },
    { opal::eAuthority::User7,  "Authority::User7"    },
    { opal::eAuthority::User8,  "Authority::User8"    },
    { opal::eAuthority::User9,  "Authority::User9"    },
    { opal::eAuthority::User10, "Authority::User10"   },
    { opal::eAuthority::User11, "Authority::User11"   },
    { opal::eAuthority::User12, "Authority::User12"   },
};

constexpr std::initializer_list<std::pair<Uid, std::string_view>> methodNames = {
    {eMethod::Properties,           "Method::Properties"         },
    { eMethod::StartSession,        "Method::StartSession"       },
    { eMethod::SyncSession,         "Method::SyncSession"        },
    { eMethod::StartTrustedSession, "Method::StartTrustedSession"},
    { eMethod::SyncTrustedSession,  "Method::SyncTrustedSession" },
    { eMethod::CloseSession,        "Method::CloseSession"       },
    { eMethod::DeleteSP,            "Method::DeleteSP"           },
    { eMethod::CreateTable,         "Method::CreateTable"        },
    { eMethod::Delete,              "Method::Delete"             },
    { eMethod::CreateRow,           "Method::CreateRow"          },
    { eMethod::DeleteRow,           "Method::DeleteRow"          },
    { eMethod::Next,                "Method::Next"               },
    { eMethod::GetFreeSpace,        "Method::GetFreeSpace"       },
    { eMethod::GetFreeRows,         "Method::GetFreeRows"        },
    { eMethod::DeleteMethod,        "Method::DeleteMethod"       },
    { eMethod::GetACL,              "Method::GetACL"             },
    { eMethod::AddACE,              "Method::AddACE"             },
    { eMethod::RemoveACE,           "Method::RemoveACE"          },
    { eMethod::GenKey,              "Method::GenKey"             },
    { eMethod::GetPackage,          "Method::GetPackage"         },
    { eMethod::SetPackage,          "Method::SetPackage"         },
    { eMethod::Get,                 "Method::Get"                },
    { eMethod::Set,                 "Method::Set"                },
    { eMethod::Authenticate,        "Method::Authenticate"       },
    { eMethod::Obsolete_0,          "Method::Obsolete_0"         },
    { eMethod::Obsolete_1,          "Method::Obsolete_1"         },
    { eMethod::Obsolete_2,          "Method::Obsolete_2"         },
    { eMethod::IssueSP,             "Method::IssueSP"            },
    { eMethod::GetClock,            "Method::GetClock"           },
    { eMethod::ResetClock,          "Method::ResetClock"         },
    { eMethod::SetClockHigh,        "Method::SetClockHigh"       },
    { eMethod::SetLagHigh,          "Method::SetLagHigh"         },
    { eMethod::SetClockLow,         "Method::SetClockLow"        },
    { eMethod::SetLagLow,           "Method::SetLagLow"          },
    { eMethod::IncrementCounter,    "Method::IncrementCounter"   },
    { eMethod::Random,              "Method::Random"             },
    { eMethod::Salt,                "Method::Salt"               },
    { eMethod::DecryptInit,         "Method::DecryptInit"        },
    { eMethod::Decrypt,             "Method::Decrypt"            },
    { eMethod::DecryptFinalize,     "Method::DecryptFinalize"    },
    { eMethod::EncryptInit,         "Method::EncryptInit"        },
    { eMethod::Encrypt,             "Method::Encrypt"            },
    { eMethod::EncryptFinalize,     "Method::EncryptFinalize"    },
    { eMethod::HMACInit,            "Method::HMACInit"           },
    { eMethod::HMAC,                "Method::HMAC"               },
    { eMethod::HMACFinalize,        "Method::HMACFinalize"       },
    { eMethod::HashInit,            "Method::HashInit"           },
    { eMethod::Hash,                "Method::Hash"               },
    { eMethod::HashFinalize,        "Method::HashFinalize"       },
    { eMethod::Sign,                "Method::Sign"               },
    { eMethod::Verify,              "Method::Verify"             },
    { eMethod::XOR,                 "Method::XOR"                },
    { eMethod::AddLog,              "Method::AddLog"             },
    { eMethod::CreateLog,           "Method::CreateLog"          },
    { eMethod::ClearLog,            "Method::ClearLog"           },
    { eMethod::FlushLog,            "Method::FlushLog"           },
    { eMethod::Reserved_0,          "Method::Reserved_0"         },
    { eMethod::Reserved_1,          "Method::Reserved_1"         },
    { eMethod::Reserved_2,          "Method::Reserved_2"         },
    { eMethod::Reserved_3,          "Method::Reserved_3"         },
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



constexpr std::initializer_list<std::pair<Uid, std::string_view>> tableC_PINNames = {
    {opal::eC_PIN::SID,             "C_PIN::SID"           },
    { opal::eC_PIN::MSID,           "C_PIN::MSID"          },
    { opal::eC_PIN::Admin1,         "C_PIN::Admin1"        },
    { opal::eC_PIN::Admin2,         "C_PIN::Admin2"        },
    { opal::eC_PIN::Admin3,         "C_PIN::Admin3"        },
    { opal::eC_PIN::Admin4,         "C_PIN::Admin4"        },
    { opal::eC_PIN::Locking_Admin1, "C_PIN::Locking_Admin1"},
    { opal::eC_PIN::Locking_Admin2, "C_PIN::Locking_Admin2"},
    { opal::eC_PIN::Locking_Admin3, "C_PIN::Locking_Admin3"},
    { opal::eC_PIN::Locking_Admin4, "C_PIN::Locking_Admin4"},
    { opal::eC_PIN::User1,          "C_PIN::User1"         },
    { opal::eC_PIN::User2,          "C_PIN::User2"         },
    { opal::eC_PIN::User3,          "C_PIN::User3"         },
    { opal::eC_PIN::User4,          "C_PIN::User4"         },
    { opal::eC_PIN::User5,          "C_PIN::User5"         },
    { opal::eC_PIN::User6,          "C_PIN::User6"         },
    { opal::eC_PIN::User7,          "C_PIN::User7"         },
    { opal::eC_PIN::User8,          "C_PIN::User8"         },
    { opal::eC_PIN::User9,          "C_PIN::User9"         },
    { opal::eC_PIN::User10,         "C_PIN::User10"        },
    { opal::eC_PIN::User11,         "C_PIN::User11"        },
    { opal::eC_PIN::User12,         "C_PIN::User12"        },
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> tableSecretProtectNames = {
    {opal::eSecretProtect::K_AES_128,  "SecretProtect::K_AES_128"},
    { opal::eSecretProtect::K_AES_256, "SecretProtect::K_AES_256"},
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> tableLockingNames = {
    {opal::eLocking::GlobalRange, "Locking::GlobalRange"},
    { opal::eLocking::Range1,     "Locking::Range1"     },
    { opal::eLocking::Range2,     "Locking::Range2"     },
    { opal::eLocking::Range3,     "Locking::Range3"     },
    { opal::eLocking::Range4,     "Locking::Range4"     },
    { opal::eLocking::Range5,     "Locking::Range5"     },
    { opal::eLocking::Range6,     "Locking::Range6"     },
    { opal::eLocking::Range7,     "Locking::Range7"     },
    { opal::eLocking::Range8,     "Locking::Range8"     },
    { opal::eLocking::Range9,     "Locking::Range9"     },
    { opal::eLocking::Range10,    "Locking::Range10"    },
    { opal::eLocking::Range11,    "Locking::Range11"    },
    { opal::eLocking::Range12,    "Locking::Range12"    },
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> tableACENames = {
    {opal::eACE::ACE_Anybody,                                              "ACE::Anybody"                                            },
    { opal::eACE::ACE_Admin,                                               "ACE::Admin"                                              },
    { opal::eACE::ACE_Anybody_Get_CommonName,                              "ACE::Anybody_Get_CommonName"                             },
    { opal::eACE::ACE_Admins_Set_CommonName,                               "ACE::Admins_Set_CommonName"                              },
    { opal::eACE::ACE_ACE_Get_All,                                         "ACE::ACE_Get_All"                                        },
    { opal::eACE::ACE_ACE_Set_BooleanExpression,                           "ACE::ACE_Set_BooleanExpression"                          },
    { opal::eACE::ACE_Authority_Get_All,                                   "ACE::Authority_Get_All"                                  },
    { opal::eACE::ACE_Authority_Set_Enabled,                               "ACE::Authority_Set_Enabled"                              },
    { opal::eACE::ACE_User1_Set_CommonName,                                "ACE::User1_Set_CommonName"                               },
    { opal::eACE::ACE_User2_Set_CommonName,                                "ACE::User2_Set_CommonName"                               },
    { opal::eACE::ACE_User3_Set_CommonName,                                "ACE::User3_Set_CommonName"                               },
    { opal::eACE::ACE_User4_Set_CommonName,                                "ACE::User4_Set_CommonName"                               },
    { opal::eACE::ACE_User5_Set_CommonName,                                "ACE::User5_Set_CommonName"                               },
    { opal::eACE::ACE_User6_Set_CommonName,                                "ACE::User6_Set_CommonName"                               },
    { opal::eACE::ACE_User7_Set_CommonName,                                "ACE::User7_Set_CommonName"                               },
    { opal::eACE::ACE_User8_Set_CommonName,                                "ACE::User8_Set_CommonName"                               },
    { opal::eACE::ACE_User9_Set_CommonName,                                "ACE::User9_Set_CommonName"                               },
    { opal::eACE::ACE_User10_Set_CommonName,                               "ACE::User10_Set_CommonName"                              },
    { opal::eACE::ACE_User11_Set_CommonName,                               "ACE::User11_Set_CommonName"                              },
    { opal::eACE::ACE_User12_Set_CommonName,                               "ACE::User12_Set_CommonName"                              },
    { opal::eACE::ACE_C_PIN_Admins_Get_All_NOPIN,                          "ACE::C_PIN_Admins_Get_All_NOPIN"                         },
    { opal::eACE::ACE_C_PIN_Admins_Set_PIN,                                "ACE::C_PIN_Admins_Set_PIN"                               },
    { opal::eACE::ACE_C_PIN_User1_Set_PIN,                                 "ACE::C_PIN_User1_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User2_Set_PIN,                                 "ACE::C_PIN_User2_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User3_Set_PIN,                                 "ACE::C_PIN_User3_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User4_Set_PIN,                                 "ACE::C_PIN_User4_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User5_Set_PIN,                                 "ACE::C_PIN_User5_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User6_Set_PIN,                                 "ACE::C_PIN_User6_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User7_Set_PIN,                                 "ACE::C_PIN_User7_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User8_Set_PIN,                                 "ACE::C_PIN_User8_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User9_Set_PIN,                                 "ACE::C_PIN_User9_Set_PIN"                                },
    { opal::eACE::ACE_C_PIN_User10_Set_PIN,                                "ACE::C_PIN_User10_Set_PIN"                               },
    { opal::eACE::ACE_C_PIN_User11_Set_PIN,                                "ACE::C_PIN_User11_Set_PIN"                               },
    { opal::eACE::ACE_C_PIN_User12_Set_PIN,                                "ACE::C_PIN_User12_Set_PIN"                               },
    { opal::eACE::ACE_K_AES_Mode,                                          "ACE::K_AES_Mode"                                         },
    { opal::eACE::ACE_K_AES_128_GlobalRange_GenKey,                        "ACE::K_AES_128_GlobalRange_GenKey"                       },
    { opal::eACE::ACE_K_AES_128_Range1_GenKey,                             "ACE::K_AES_128_Range1_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range2_GenKey,                             "ACE::K_AES_128_Range2_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range3_GenKey,                             "ACE::K_AES_128_Range3_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range4_GenKey,                             "ACE::K_AES_128_Range4_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range5_GenKey,                             "ACE::K_AES_128_Range5_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range6_GenKey,                             "ACE::K_AES_128_Range6_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range7_GenKey,                             "ACE::K_AES_128_Range7_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range8_GenKey,                             "ACE::K_AES_128_Range8_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range9_GenKey,                             "ACE::K_AES_128_Range9_GenKey"                            },
    { opal::eACE::ACE_K_AES_128_Range10_GenKey,                            "ACE::K_AES_128_Range10_GenKey"                           },
    { opal::eACE::ACE_K_AES_128_Range11_GenKey,                            "ACE::K_AES_128_Range11_GenKey"                           },
    { opal::eACE::ACE_K_AES_128_Range12_GenKey,                            "ACE::K_AES_128_Range12_GenKey"                           },
    { opal::eACE::ACE_K_AES_256_GlobalRange_GenKey,                        "ACE::K_AES_256_GlobalRange_GenKey"                       },
    { opal::eACE::ACE_K_AES_256_Range1_GenKey,                             "ACE::K_AES_256_Range1_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range2_GenKey,                             "ACE::K_AES_256_Range2_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range3_GenKey,                             "ACE::K_AES_256_Range3_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range4_GenKey,                             "ACE::K_AES_256_Range4_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range5_GenKey,                             "ACE::K_AES_256_Range5_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range6_GenKey,                             "ACE::K_AES_256_Range6_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range7_GenKey,                             "ACE::K_AES_256_Range7_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range8_GenKey,                             "ACE::K_AES_256_Range8_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range9_GenKey,                             "ACE::K_AES_256_Range9_GenKey"                            },
    { opal::eACE::ACE_K_AES_256_Range10_GenKey,                            "ACE::K_AES_256_Range10_GenKey"                           },
    { opal::eACE::ACE_K_AES_256_Range11_GenKey,                            "ACE::K_AES_256_Range11_GenKey"                           },
    { opal::eACE::ACE_K_AES_256_Range12_GenKey,                            "ACE::K_AES_256_Range12_GenKey"                           },
    { opal::eACE::ACE_Locking_GlobalRange_Get_RangeStartToActiveKey,       "ACE::Locking_GlobalRange_Get_RangeStartToActiveKey"      },
    { opal::eACE::ACE_Locking_Range1_Get_RangeStartToActiveKey,            "ACE::Locking_Range1_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range2_Get_RangeStartToActiveKey,            "ACE::Locking_Range2_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range3_Get_RangeStartToActiveKey,            "ACE::Locking_Range3_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range4_Get_RangeStartToActiveKey,            "ACE::Locking_Range4_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range5_Get_RangeStartToActiveKey,            "ACE::Locking_Range5_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range6_Get_RangeStartToActiveKey,            "ACE::Locking_Range6_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range7_Get_RangeStartToActiveKey,            "ACE::Locking_Range7_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range8_Get_RangeStartToActiveKey,            "ACE::Locking_Range8_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range9_Get_RangeStartToActiveKey,            "ACE::Locking_Range9_Get_RangeStartToActiveKey"           },
    { opal::eACE::ACE_Locking_Range10_Get_RangeStartToActiveKey,           "ACE::Locking_Range10_Get_RangeStartToActiveKey"          },
    { opal::eACE::ACE_Locking_Range11_Get_RangeStartToActiveKey,           "ACE::Locking_Range11_Get_RangeStartToActiveKey"          },
    { opal::eACE::ACE_Locking_Range12_Get_RangeStartToActiveKey,           "ACE::Locking_Range12_Get_RangeStartToActiveKey"          },
    { opal::eACE::ACE_Locking_GlobalRange_Set_RdLocked,                    "ACE::Locking_GlobalRange_Set_RdLocked"                   },
    { opal::eACE::ACE_Locking_Range1_Set_RdLocked,                         "ACE::Locking_Range1_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range2_Set_RdLocked,                         "ACE::Locking_Range2_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range3_Set_RdLocked,                         "ACE::Locking_Range3_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range4_Set_RdLocked,                         "ACE::Locking_Range4_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range5_Set_RdLocked,                         "ACE::Locking_Range5_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range6_Set_RdLocked,                         "ACE::Locking_Range6_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range7_Set_RdLocked,                         "ACE::Locking_Range7_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range8_Set_RdLocked,                         "ACE::Locking_Range8_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range9_Set_RdLocked,                         "ACE::Locking_Range9_Set_RdLocked"                        },
    { opal::eACE::ACE_Locking_Range10_Set_RdLocked,                        "ACE::Locking_Range10_Set_RdLocked"                       },
    { opal::eACE::ACE_Locking_Range11_Set_RdLocked,                        "ACE::Locking_Range11_Set_RdLocked"                       },
    { opal::eACE::ACE_Locking_Range12_Set_RdLocked,                        "ACE::Locking_Range12_Set_RdLocked"                       },
    { opal::eACE::ACE_Locking_GlobalRange_Set_WrLocked,                    "ACE::Locking_GlobalRange_Set_WrLocked"                   },
    { opal::eACE::ACE_Locking_Range1_Set_WrLocked,                         "ACE::Locking_Range1_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range2_Set_WrLocked,                         "ACE::Locking_Range2_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range3_Set_WrLocked,                         "ACE::Locking_Range3_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range4_Set_WrLocked,                         "ACE::Locking_Range4_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range5_Set_WrLocked,                         "ACE::Locking_Range5_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range6_Set_WrLocked,                         "ACE::Locking_Range6_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range7_Set_WrLocked,                         "ACE::Locking_Range7_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range8_Set_WrLocked,                         "ACE::Locking_Range8_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range9_Set_WrLocked,                         "ACE::Locking_Range9_Set_WrLocked"                        },
    { opal::eACE::ACE_Locking_Range10_Set_WrLocked,                        "ACE::Locking_Range10_Set_WrLocked"                       },
    { opal::eACE::ACE_Locking_Range11_Set_WrLocked,                        "ACE::Locking_Range11_Set_WrLocked"                       },
    { opal::eACE::ACE_Locking_Range12_Set_WrLocked,                        "ACE::Locking_Range12_Set_WrLocked"                       },
    { opal::eACE::ACE_Locking_GlblRng_Admins_Set,                          "ACE::Locking_GlblRng_Admins_Set"                         },
    { opal::eACE::ACE_Locking_Admins_RangeStartToLOR,                      "ACE::Locking_Admins_RangeStartToLOR"                     },
    { opal::eACE::ACE_MBRControl_Admins_Set,                               "ACE::MBRControl_Admins_Set"                              },
    { opal::eACE::ACE_MBRControl_Set_DoneToDOR,                            "ACE::MBRControl_Set_DoneToDOR"                           },
    { opal::eACE::ACE_DataStore_Get_All,                                   "ACE::DataStore_Get_All"                                  },
    { opal::eACE::ACE_DataStore_Set_All,                                   "ACE::DataStore_Set_All"                                  },
    { opal::eACE::ACE_Set_Enabled,                                         "ACE::Set_Enabled"                                        },
    { opal::eACE::ACE_C_PIN_SID_Get_NOPIN,                                 "ACE::C_PIN_SID_Get_NOPIN"                                },
    { opal::eACE::ACE_C_PIN_SID_Set_PIN,                                   "ACE::C_PIN_SID_Set_PIN"                                  },
    { opal::eACE::ACE_C_PIN_MSID_Get_PIN,                                  "ACE::C_PIN_MSID_Get_PIN"                                 },
    { opal::eACE::ACE_TPerInfo_Set_ProgrammaticResetEnable,                "ACE::TPerInfo_Set_ProgrammaticResetEnable"               },
    { opal::eACE::ACE_SP_SID,                                              "ACE::SP_SID"                                             },
    { opal::eACE::ACE_DataRemovalMechanism_Set_ActiveDataRemovalMechanism, "ACE::DataRemovalMechanism_Set_ActiveDataRemovalMechanism"},
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> tableSingleRowNames = {
    {eTableSingleRowObject::SPInfo,       "SPInfo::SPInfo"          },
    { eTableSingleRowObject::TPerInfo,    "TPerInfo::TPerInfo"      },
    { eTableSingleRowObject::LockingInfo, "LockingInfo::LockingInfo"},
    { eTableSingleRowObject::MBRControl,  "MBRControl::MBRControl"  },
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
        amend(mapping, tableC_PINNames);
        amend(mapping, tableSecretProtectNames);
        amend(mapping, tableACENames);
        amend(mapping, tableLockingNames);
        amend(mapping, tableSingleRowNames);
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
        amend(mapping, tableC_PINNames);
        amend(mapping, tableSecretProtectNames);
        amend(mapping, tableACENames);
        amend(mapping, tableLockingNames);
        amend(mapping, tableSingleRowNames);
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
        size_t idx = 0;
        const uint64_t value = std::bit_cast<uint64_t>(std::stoll(std::string(name), &idx, 16));
        if (idx != name.size()) {
            return std::nullopt;
        }
        return Uid(value);
    }
    catch (std::exception& ex) {
        return std::nullopt;
    }
}

std::vector<std::string_view> SplitName(std::string_view name) {
    std::vector<std::string_view> sections;
    size_t pos = 0;
    while (pos <= name.size()) {
        const auto start = pos;
        pos = name.find("::", pos);
        sections.push_back(name.substr(start, pos));
        pos = std::max(pos + 2, pos);
    }
    return sections;
}