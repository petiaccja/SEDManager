#include "OpalModule.hpp"

#include "../Common/Utility.hpp"
#include "../Core/CoreModule.hpp"
#include "../PSID/PSIDModule.hpp"

#include <ranges>


namespace sedmgr {

namespace opal {


    constexpr std::initializer_list<std::pair<UID, std::string_view>> methods = {
        {UID(eMethod::Activate),  "MethodID::Activate"},
        { UID(eMethod::Revert),   "MethodID::Revert"  },
        { UID(eMethod::RevertSP), "MethodID::RevertSP"},
    };

    constexpr std::initializer_list<std::pair<UID, std::string_view>> tables = {
        {UID(eTable::DataStore),             "DataStore"           },
        { UID(eTable::DataRemovalMechanism), "DataRemovalMechanism"},
    };

    constexpr std::initializer_list<std::pair<UID, std::string_view>> tableDescriptors = {
        {UID(eTable::DataStore).ToDescriptor(),             "Table::DataStore"           },
        { UID(eTable::DataRemovalMechanism).ToDescriptor(), "Table::DataRemovalMechanism"},
    };

    static const std::initializer_list<std::pair<UID, TableDescStatic>> tableDescs = {
        {UID(eTable::DataStore), TableDescStatic{ "DataStore", eTableKind::BYTE }},
    };

    namespace preconf {

        enum class eSP : uint64_t {
            Admin = 0x0000'0205'0000'0001,
            Locking = 0x0000'0205'0000'0002,
        };


        namespace admin {

            constexpr std::initializer_list<std::pair<UID, std::string_view>> spInfo = {
                {0x0000'0002'0000'0001_uid, "SPInfo::Admin"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> spTemplates = {
                {0x0000'0003'0000'0001_uid,  "SPTemplates::Base" },
                { 0x0000'0003'0000'0002_uid, "SPTemplates::Admin"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> ace = {
                {0x0000'0008'0000'0001_uid,  "ACE::Anybody"                                            },
                { 0x0000'0008'0000'0002_uid, "ACE::Admin"                                              },
                { 0x0000'0008'0003'0001_uid, "ACE::Set_Enabled"                                        },
                { 0x0000'0008'0000'8C02_uid, "ACE::C_PIN_SID_Get_NOPIN"                                },
                { 0x0000'0008'0000'8C03_uid, "ACE::C_PIN_SID_Set_PIN"                                  },
                { 0x0000'0008'0000'8C04_uid, "ACE::C_PIN_MSID_Get_PIN"                                 },
                { 0x0000'0008'0003'A001_uid, "ACE::C_PIN_Admins_Set_PIN"                               },
                { 0x0000'0008'0003'0003_uid, "ACE::TPerInfo_Set_ProgrammaticResetEnable"               },
                { 0x0000'0008'0003'0002_uid, "ACE::SP_SID"                                             },
                { 0x0000'0008'0005'0001_uid, "ACE::DataRemovalMechanism_Set_ActiveDataRemovalMechanism"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> authority = {
                {0x0000'0009'0000'0001_uid,  "Authority::Anybody"},
                { 0x0000'0009'0000'0002_uid, "Authority::Admins" },
                { 0x0000'0009'0000'0003_uid, "Authority::Makers" },
                { 0x0000'0009'0000'0006_uid, "Authority::SID"    },
            };
            constexpr std::initializer_list<NameSequence> authoritySeq = {
                {0x0000'0009'0000'0201_uid, 1, 32, "Authority::Admin{}"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> cPin = {
                {0x0000'000B'0000'0001_uid,  "C_PIN::SID" },
                { 0x0000'000B'0000'8402_uid, "C_PIN::MSID"},
            };
            constexpr std::initializer_list<NameSequence> cPinSeq = {
                {0x0000'000B'0000'0201_uid, 1, 32, "C_PIN::Admin{}"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> template_ = {
                {0x0000'0204'0000'0001_uid,  "Template::Base"   },
                { 0x0000'0204'0000'0002_uid, "Template::Admin"  },
                { 0x0000'0204'0000'0006_uid, "Template::Locking"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> sp = {
                {UID(eSP::Admin),    "SP::Admin"  },
                { UID(eSP::Locking), "SP::Locking"},
            };

        } // namespace admin

        namespace locking {
            constexpr std::initializer_list<std::pair<UID, std::string_view>> spInfo = {
                {0x0000'0002'0000'0001_uid, "SPInfo::Locking"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> spTemplates = {
                {0x0000'0003'0000'0001_uid,  "SPTemplates::Base"   },
                { 0x0000'0003'0000'0002_uid, "SPTemplates::Locking"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> ace = {
                {0x0000'0008'0000'0001_uid,  "ACE::Anybody"                                            },
                { 0x0000'0008'0000'0002_uid, "ACE::Admin"                                              },
                { 0x0000'0008'0000'0003_uid, "ACE::Anybody_Get_CommonName"                             },
                { 0x0000'0008'0000'0004_uid, "ACE::Admins_Set_CommonName"                              },
                { 0x0000'0008'0003'8000_uid, "ACE::ACE_Get_All"                                        },
                { 0x0000'0008'0003'8001_uid, "ACE::ACE_Set_BooleanExpression"                          },
                { 0x0000'0008'0003'9000_uid, "ACE::Authority_Get_All"                                  },
                { 0x0000'0008'0003'9001_uid, "ACE::Authority_Set_Enabled"                              },
                { 0x0000'0008'0003'A000_uid, "ACE::C_PIN_Admins_Get_All_NOPIN"                         },
                { 0x0000'0008'0003'A001_uid, "ACE::C_PIN_Admins_Set_PIN"                               },
                { 0x0000'0008'0003'BFFF_uid, "ACE::K_AES_Mode"                                         },
                { 0x0000'0008'0003'B000_uid, "ACE::K_AES_128_GlobalRange_GenKey"                       },
                { 0x0000'0008'0003'B800_uid, "ACE::K_AES_256_GlobalRange_GenKey"                       },
                { 0x0000'0008'0003'D000_uid, "ACE::Locking_GlobalRange_Get_RangeStartToActiveKey"      },
                { 0x0000'0008'0003'E000_uid, "ACE::Locking_GlobalRange_Set_RdLocked"                   },
                { 0x0000'0008'0003'E800_uid, "ACE::Locking_GlobalRange_Set_WrLocked"                   },
                { 0x0000'0008'0003'F000_uid, "ACE::Locking_GlblRng_Admins_Set"                         },
                { 0x0000'0008'0003'F001_uid, "ACE::Locking_Admins_RangeStartToLOR"                     },
                { 0x0000'0008'0003'F800_uid, "ACE::MBRControl_Admins_Set"                              },
                { 0x0000'0008'0003'F801_uid, "ACE::MBRControl_Set_DoneToDOR"                           },
                { 0x0000'0008'0003'FC00_uid, "ACE::DataStore_Get_All"                                  },
                { 0x0000'0008'0003'FC01_uid, "ACE::DataStore_Set_All"                                  },
                { 0x0000'0008'0003'0001_uid, "ACE::Set_Enabled"                                        },
                { 0x0000'0008'0000'8C02_uid, "ACE::C_PIN_SID_Get_NOPIN"                                },
                { 0x0000'0008'0000'8C03_uid, "ACE::C_PIN_SID_Set_PIN"                                  },
                { 0x0000'0008'0000'8C04_uid, "ACE::C_PIN_MSID_Get_PIN"                                 },
                { 0x0000'0008'0003'0003_uid, "ACE::TPerInfo_Set_ProgrammaticResetEnable"               },
                { 0x0000'0008'0003'0002_uid, "ACE::SP_SID"                                             },
                { 0x0000'0008'0005'0001_uid, "ACE::DataRemovalMechanism_Set_ActiveDataRemovalMechanism"},
            };
            constexpr std::initializer_list<NameSequence> aceSeq = {
                {0x0000'0008'0004'4001_uid,  1, 32, "ACE::User{}_Set_CommonName"                    },
                { 0x0000'0008'0003'A801_uid, 1, 32, "ACE::C_PIN_User{}_Set_PIN"                     },
                { 0x0000'0008'0003'B001_uid, 1, 32, "ACE::K_AES_128_Range{}_GenKey"                 },
                { 0x0000'0008'0003'B801_uid, 1, 32, "ACE::K_AES_256_Range{}_GenKey"                 },
                { 0x0000'0008'0003'D001_uid, 1, 32, "ACE::Locking_Range{}_Get_RangeStartToActiveKey"},
                { 0x0000'0008'0003'E001_uid, 1, 32, "ACE::Locking_Range{}_Set_RdLocked"             },
                { 0x0000'0008'0003'E801_uid, 1, 32, "ACE::Locking_Range{}_Set_WrLocked"             },
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> authority = {
                {0x0000'0009'0000'0001_uid,  "Authority::Anybody"},
                { 0x0000'0009'0000'0002_uid, "Authority::Admins" },
                { 0x0000'0009'0003'0000_uid, "Authority::Users"  },
            };
            constexpr std::initializer_list<NameSequence> authoritySeq = {
                {0x0000'0009'0001'0001_uid,  1, 32, "Authority::Admin{}"},
                { 0x0000'0009'0003'0001_uid, 1, 32, "Authority::User{}" },
            };
            constexpr std::initializer_list<NameSequence> cPinSeq = {
                {0x0000'000B'0001'0001_uid,  1, 32, "C_PIN::Admin{}"},
                { 0x0000'000B'0003'0001_uid, 1, 32, "C_PIN::User{}" },
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> secretProtect = {
                {0x0000'001D'0000'001D_uid,  "SecretProtect::K_AES_128"},
                { 0x0000'001D'0000'001E_uid, "SecretProtect::K_AES_256"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> locking = {
                {0x0000'0802'0000'0001_uid, "Locking::GlobalRange"},
            };
            constexpr std::initializer_list<NameSequence> lockingSeq = {
                {0x0000'0802'0003'0001_uid, 1, 32, "Locking::Range{}"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> kAes128 = {
                {0x0000'0805'0000'0001_uid, "K_AES_128::GlobalRange"},
            };
            constexpr std::initializer_list<NameSequence> kAes128Seq = {
                {0x0000'0805'0003'0001_uid, 1, 32, "K_AES_128::Range{}"},
            };
            constexpr std::initializer_list<std::pair<UID, std::string_view>> kAes256 = {
                {0x0000'0806'0000'0001_uid, "K_AES_256::GlobalRange"},
            };
            constexpr std::initializer_list<NameSequence> kAes256Seq = {
                {0x0000'0806'0003'0001_uid, 1, 32, "K_AES_256::Range{}"},
            };

        } // namespace locking

    } // namespace preconf


    const SPNameAndUidFinder& GetFinder() {
        static const auto globalPairs = {
            methods,
            tables,
            tableDescriptors,
            preconf::admin::sp
        };

        static const auto lockingPairs = {
            preconf::locking::spInfo,
            preconf::locking::spTemplates,
            preconf::locking::ace,
            preconf::locking::authority,
            preconf::locking::secretProtect,
            preconf::locking::locking,
            preconf::locking::kAes128,
            preconf::locking::kAes256,
        };

        static const auto lockingSequences = {
            preconf::locking::aceSeq,
            preconf::locking::authoritySeq,
            preconf::locking::cPinSeq,
            preconf::locking::lockingSeq,
            preconf::locking::kAes128Seq,
            preconf::locking::kAes256Seq,
        };

        static const auto adminPairs = {
            preconf::admin::spInfo,
            preconf::admin::spTemplates,
            preconf::admin::ace,
            preconf::admin::authority,
            preconf::admin::cPin,
            preconf::admin::template_,
        };

        static const auto adminSequences = {
            preconf::admin::authoritySeq,
            preconf::admin::cPinSeq,
        };

        static SPNameAndUidFinder finder({
            {UID(0),                      NameAndUidFinder(globalPairs,                                                {})                                 },
            { UID(preconf::eSP::Admin),   NameAndUidFinder(std::array{ adminPairs, globalPairs } | std::views::join,   adminSequences | std::views::join)  },
            { UID(preconf::eSP::Locking), NameAndUidFinder(std::array{ lockingPairs, globalPairs } | std::views::join, lockingSequences | std::views::join)},
        });
        return finder;
    }


} // namespace opal


std::shared_ptr<Module> Opal1Module::Get() {
    static const auto instance = std::make_shared<Opal1Module>();
    return instance;
}


std::string_view Opal1Module::ModuleName() const {
    return "Opal v1";
}


std::shared_ptr<Module> Opal2Module::Get() {
    static const auto instance = std::make_shared<Opal2Module>();
    return instance;
}


std::string_view Opal2Module::ModuleName() const {
    return "Opal v2";
}


eModuleKind OpalModule::ModuleKind() const {
    return eModuleKind::SSC;
}


std::optional<std::string> OpalModule::FindName(UID uid, std::optional<UID> sp) const {
    auto result = opal::GetFinder().Find(uid, sp.value_or(0_uid));
    if (result) {
        return result;
    }
    return opal::GetFinder().Find(uid, UID(0));
}


std::optional<UID> OpalModule::FindUid(std::string_view name, std::optional<UID> sp) const {
    auto result = opal::GetFinder().Find(name, sp.value_or(0_uid));
    if (result) {
        return result;
    }
    return opal::GetFinder().Find(name, UID(0));
}


std::optional<TableDesc> OpalModule::FindTable(UID table) const {
    if (table == UID(core::eTable::TPerInfo)) {
        auto desc = CoreModule::Get()->FindTable(table);
        desc->columns.push_back({ "ProgrammaticResetEnable", false, CoreModule::Get()->FindType(UID(core::eType::boolean)).value() });
        return desc;
    }
    if (table == UID(core::eTable::Table)) {
        auto desc = CoreModule::Get()->FindTable(table);
        desc->columns.push_back({ "MandatoryWriteGranularity", false, CoreModule::Get()->FindType(UID(core::eType::uinteger_4)).value() });
        desc->columns.push_back({ "RecommendedAccessGranularity", false, CoreModule::Get()->FindType(UID(core::eType::uinteger_4)).value() });
        return desc;
    }
    return std::nullopt;
}


std::optional<Type> OpalModule::FindType(UID uid) const {
    return std::nullopt;
}


std::span<const std::shared_ptr<Module>> OpalModule::Features() const {
    static const std::vector<std::shared_ptr<Module>> features = {
        PSIDModule::Get(),
    };
    return features;
}
} // namespace sedmgr