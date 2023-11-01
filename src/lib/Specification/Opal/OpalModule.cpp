#include "OpalModule.hpp"

#include "../Common/Utility.hpp"
#include "../Core/CoreModule.hpp"
#include "../Core/Defs/Types.hpp"
#include "../PSID/PSIDModule.hpp"

#include <ranges>


namespace sedmgr {

namespace opal {


    constexpr std::initializer_list<std::pair<Uid, std::string_view>> methods = {
        {opal::eMethod::Activate, "MethodID::Activate"},
        { opal::eMethod::Revert,  "MethodID::Revert"  },
    };


    namespace preconf {

        enum class eSP : uint64_t {
            Admin = 0x0000'0205'0000'0001,
            Locking = 0x0000'0205'0000'0002,
        };


        namespace admin {

            constexpr std::initializer_list<std::pair<Uid, std::string_view>> spInfo = {
                {0x0000'0002'0000'0001, "SPInfo::Admin"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> spTemplates = {
                {0x0000'0003'0000'0001,  "SPTemplates::Base" },
                { 0x0000'0003'0000'0002, "SPTemplates::Admin"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> ace = {
                {0x0000'0008'0000'0001,  "ACE::Anybody"                                            },
                { 0x0000'0008'0000'0002, "ACE::Admin"                                              },
                { 0x0000'0008'0003'0001, "ACE::Set_Enabled"                                        },
                { 0x0000'0008'0000'8C02, "ACE::C_PIN_SID_Get_NOPIN"                                },
                { 0x0000'0008'0000'8C03, "ACE::C_PIN_SID_Set_PIN"                                  },
                { 0x0000'0008'0000'8C04, "ACE::C_PIN_MSID_Get_PIN"                                 },
                { 0x0000'0008'0003'A001, "ACE::C_PIN_Admins_Set_PIN"                               },
                { 0x0000'0008'0003'0003, "ACE::TPerInfo_Set_ProgrammaticResetEnable"               },
                { 0x0000'0008'0003'0002, "ACE::SP_SID"                                             },
                { 0x0000'0008'0005'0001, "ACE::DataRemovalMechanism_Set_ActiveDataRemovalMechanism"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> authority = {
                {0x0000'0009'0000'0001,  "Authority::Anybody"},
                { 0x0000'0009'0000'0002, "Authority::Admins" },
                { 0x0000'0009'0000'0003, "Authority::Makers" },
                { 0x0000'0009'0000'0006, "Authority::SID"    },
            };
            const std::initializer_list<NameSequence> authoritySeq = {
                {0x0000'0009'0000'0201, 1, 32, "Authority::Admin{}"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> cPin = {
                {0x0000'000B'0000'0001,  "C_PIN::SID" },
                { 0x0000'000B'0000'8402, "C_PIN::MSID"},
            };
            const std::initializer_list<NameSequence> cPinSeq = {
                {0x0000'000B'0000'0201, 1, 32, "C_PIN::Admin{}"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> template_ = {
                {0x0000'0204'0000'0001,  "Template::Base"   },
                { 0x0000'0204'0000'0002, "Template::Admin"  },
                { 0x0000'0204'0000'0006, "Template::Locking"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> sp = {
                {eSP::Admin,    "SP::Admin"  },
                { eSP::Locking, "SP::Locking"},
            };

        } // namespace admin

        namespace locking {
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> spInfo = {
                {0x0000'0002'0000'0001, "SPInfo::Locking"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> spTemplates = {
                {0x0000'0003'0000'0001,  "SPTemplates::Base"   },
                { 0x0000'0003'0000'0002, "SPTemplates::Locking"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> ace = {
                {0x0000'0008'0000'0001,  "ACE::Anybody"                                            },
                { 0x0000'0008'0000'0002, "ACE::Admin"                                              },
                { 0x0000'0008'0000'0003, "ACE::Anybody_Get_CommonName"                             },
                { 0x0000'0008'0000'0004, "ACE::Admins_Set_CommonName"                              },
                { 0x0000'0008'0003'8000, "ACE::ACE_Get_All"                                        },
                { 0x0000'0008'0003'8001, "ACE::ACE_Set_BooleanExpression"                          },
                { 0x0000'0008'0003'9000, "ACE::Authority_Get_All"                                  },
                { 0x0000'0008'0003'9001, "ACE::Authority_Set_Enabled"                              },
                { 0x0000'0008'0003'A000, "ACE::C_PIN_Admins_Get_All_NOPIN"                         },
                { 0x0000'0008'0003'A001, "ACE::C_PIN_Admins_Set_PIN"                               },
                { 0x0000'0008'0003'BFFF, "ACE::K_AES_Mode"                                         },
                { 0x0000'0008'0003'B000, "ACE::K_AES_128_GlobalRange_GenKey"                       },
                { 0x0000'0008'0003'B800, "ACE::K_AES_256_GlobalRange_GenKey"                       },
                { 0x0000'0008'0003'D000, "ACE::Locking_GlobalRange_Get_RangeStartToActiveKey"      },
                { 0x0000'0008'0003'E000, "ACE::Locking_GlobalRange_Set_RdLocked"                   },
                { 0x0000'0008'0003'E800, "ACE::Locking_GlobalRange_Set_WrLocked"                   },
                { 0x0000'0008'0003'F000, "ACE::Locking_GlblRng_Admins_Set"                         },
                { 0x0000'0008'0003'F001, "ACE::Locking_Admins_RangeStartToLOR"                     },
                { 0x0000'0008'0003'F800, "ACE::MBRControl_Admins_Set"                              },
                { 0x0000'0008'0003'F801, "ACE::MBRControl_Set_DoneToDOR"                           },
                { 0x0000'0008'0003'FC00, "ACE::DataStore_Get_All"                                  },
                { 0x0000'0008'0003'FC01, "ACE::DataStore_Set_All"                                  },
                { 0x0000'0008'0003'0001, "ACE::Set_Enabled"                                        },
                { 0x0000'0008'0000'8C02, "ACE::C_PIN_SID_Get_NOPIN"                                },
                { 0x0000'0008'0000'8C03, "ACE::C_PIN_SID_Set_PIN"                                  },
                { 0x0000'0008'0000'8C04, "ACE::C_PIN_MSID_Get_PIN"                                 },
                { 0x0000'0008'0003'0003, "ACE::TPerInfo_Set_ProgrammaticResetEnable"               },
                { 0x0000'0008'0003'0002, "ACE::SP_SID"                                             },
                { 0x0000'0008'0005'0001, "ACE::DataRemovalMechanism_Set_ActiveDataRemovalMechanism"},
            };
            const std::initializer_list<NameSequence> aceSeq = {
                {0x0000'0008'0004'4001,  1, 32, "ACE::User{}_Set_CommonName"                    },
                { 0x0000'0008'0003'A801, 1, 32, "ACE::C_PIN_User{}_Set_PIN"                     },
                { 0x0000'0008'0003'B001, 1, 32, "ACE::K_AES_128_Range{}_GenKey"                 },
                { 0x0000'0008'0003'B801, 1, 32, "ACE::K_AES_256_Range{}_GenKey"                 },
                { 0x0000'0008'0003'D001, 1, 32, "ACE::Locking_Range{}_Get_RangeStartToActiveKey"},
                { 0x0000'0008'0003'E001, 1, 32, "ACE::Locking_Range{}_Set_RdLocked"             },
                { 0x0000'0008'0003'E801, 1, 32, "ACE::Locking_Range{}_Set_WrLocked"             },
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> authority = {
                {0x0000'0009'0000'0001,  "Authority::Anybody"},
                { 0x0000'0009'0000'0002, "Authority::Admins" },
                { 0x0000'0009'0003'0000, "Authority::Users"  },
            };
            const std::initializer_list<NameSequence> authoritySeq = {
                {0x0000'0009'0001'0001,  1, 32, "Authority::Admin{}"},
                { 0x0000'0009'0003'0001, 1, 32, "Authority::User{}" },
            };
            const std::initializer_list<NameSequence> cPinSeq = {
                {0x0000'000B'0001'0001,  1, 32, "C_PIN::Admin{}"},
                { 0x0000'000B'0003'0001, 1, 32, "C_PIN::User{}" },
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> secretProtect = {
                {0x0000'001D'0000'001D,  "SecretProtect::K_AES_128"},
                { 0x0000'001D'0000'001E, "SecretProtect::K_AES_256"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> locking = {
                {0x0000'0802'0000'0001, "Locking::GlobalRange"},
            };
            const std::initializer_list<NameSequence> lockingSeq = {
                {0x0000'0802'0003'0001, 1, 32, "Locking::Range{}"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> kAes128 = {
                {0x0000'0805'0000'0001, "K_AES_128::GlobalRange"},
            };
            const std::initializer_list<NameSequence> kAes128Seq = {
                {0x0000'0805'0003'0001, 1, 32, "K_AES_128::Range{}"},
            };
            constexpr std::initializer_list<std::pair<Uid, std::string_view>> kAes256 = {
                {0x0000'0806'0000'0001, "K_AES_256::GlobalRange"},
            };
            const std::initializer_list<NameSequence> kAes256Seq = {
                {0x0000'0806'0003'0001, 1, 32, "K_AES_256::Range{}"},
            };

        } // namespace locking

    } // namespace preconf


    const SPNameAndUidFinder& GetFinder() {
        const auto globalPairs = {
            methods,
            preconf::admin::sp
        };

        const auto lockingPairs = {
            preconf::locking::spInfo,
            preconf::locking::spTemplates,
            preconf::locking::ace,
            preconf::locking::authority,
            preconf::locking::secretProtect,
            preconf::locking::locking,
            preconf::locking::kAes128,
            preconf::locking::kAes256,
        };

        const auto lockingSequences = {
            preconf::locking::aceSeq,
            preconf::locking::authoritySeq,
            preconf::locking::cPinSeq,
            preconf::locking::lockingSeq,
            preconf::locking::kAes128Seq,
            preconf::locking::kAes256Seq,
        };

        const auto adminPairs = {
            preconf::admin::spInfo,
            preconf::admin::spTemplates,
            preconf::admin::ace,
            preconf::admin::authority,
            preconf::admin::cPin,
            preconf::admin::template_,
            preconf::admin::sp,
        };

        const auto adminSequences = {
            preconf::admin::authoritySeq,
            preconf::admin::cPinSeq,
        };

        static SPNameAndUidFinder finder({
            {Uid(0),                      NameAndUidFinder(globalPairs,  {})                                 },
            { Uid(preconf::eSP::Admin),   NameAndUidFinder(adminPairs,   adminSequences | std::views::join)  },
            { Uid(preconf::eSP::Locking), NameAndUidFinder(lockingPairs, lockingSequences | std::views::join)},
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


std::optional<std::string> OpalModule::FindName(Uid uid, std::optional<Uid> sp) const {
    auto result = opal::GetFinder().Find(uid, sp.value_or(0));
    if (result) {
        return result;
    }
    return opal::GetFinder().Find(uid, Uid(0));
}


std::optional<Uid> OpalModule::FindUid(std::string_view name, std::optional<Uid> sp) const {
    auto result = opal::GetFinder().Find(name, sp.value_or(0));
    if (result) {
        return result;
    }
    return opal::GetFinder().Find(name, Uid(0));
}


std::optional<TableDesc> OpalModule::FindTable(Uid table) const {
    if (table == Uid(core::eTable::TPerInfo)) {
        auto desc = CoreModule::Get()->FindTable(table);
        desc->columns.push_back({ "ProgrammaticResetEnable", false, core::boolean });
        return desc;
    }
    return std::nullopt;
}


std::optional<Type> OpalModule::FindType(Uid uid) const {
    return std::nullopt;
}


std::span<const std::shared_ptr<Module>> OpalModule::Features() const {
    static const std::vector<std::shared_ptr<Module>> features = {
        PSIDModule::Get(),
    };
    return features;
}
} // namespace sedmgr