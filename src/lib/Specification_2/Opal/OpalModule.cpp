#include "OpalModule.hpp"

#include "../Common/Utility.hpp"
#include "../PSID/PSIDModule.hpp"

#include <ranges>


namespace opal {


constexpr std::initializer_list<std::pair<Uid, std::string_view>> methods = {
    {opal::eMethod::Activate, "Activate"},
    { opal::eMethod::Revert,  "Revert"  },
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> preconfSP = {
    {0x0000'0205'0000'0001,  "Admin"  },
    { 0x0000'0205'0000'0002, "Locking"},
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> preconfAuthority = {
    {0x0000'0009'0001'FF01,  "PSID" },
    { 0x0000'0009'0003'0000, "Users"},
};

const std::initializer_list<NameSequence> preconfAuthoritySeq = {
    {0x0000'0009'0001'0001,  1, 32, "Admin{}"},
    { 0x0000'0009'0003'0001, 1, 32, "User{}" },
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> preconfC_PIN = {
    {0x0000'000B'0000'0001,  "SID" },
    { 0x0000'000B'0000'8402, "MSID"},
};

const std::initializer_list<NameSequence> preconfC_PINSeq = {
    {0x0000'000B'0000'0201,  1, 32, "Admin{}"        },
    { 0x0000'000B'0001'0001, 1, 32, "Locking_Admin{}"},
    { 0x0000'000B'0003'0001, 1, 32, "User{}"         },
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> preconfSecretProtect = {
    {0x0000'001D'0000'001D,  "K_AES_128"},
    { 0x0000'001D'0000'001E, "K_AES_256"},
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> preconfLocking = {
    {0x0000'0802'0000'0001, "GlobalRange"},
};

const std::initializer_list<NameSequence> preconfLockingSeq = {
    {0x0000'0802'0003'0001, 1, 32, "Range{}"},
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> preconfACE = {
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


const std::initializer_list<NameSequence> preconfACESeq = {
    {0x0000'0008'0004'4001,  1, 32, "ACE::User{}_Set_CommonName"                    },
    { 0x0000'0008'0003'A801, 1, 32, "ACE::C_PIN_User{}_Set_PIN"                     },
    { 0x0000'0008'0003'B001, 1, 32, "ACE::K_AES_128_Range{}_GenKey"                 },
    { 0x0000'0008'0003'B801, 1, 32, "ACE::K_AES_256_Range{}_GenKey"                 },
    { 0x0000'0008'0003'D001, 1, 32, "ACE::Locking_Range{}_Get_RangeStartToActiveKey"},
    { 0x0000'0008'0003'E001, 1, 32, "ACE::Locking_Range{}_Set_RdLocked"             },
    { 0x0000'0008'0003'E801, 1, 32, "ACE::Locking_Range{}_Set_WrLocked"             },
};


const auto allNames = {
    preconfSP,
    preconfAuthority,
    preconfC_PIN,
    preconfSecretProtect,
    preconfLocking,
    preconfACE,
    methods,
};


const auto allNameSequences = {
    preconfAuthoritySeq,
    preconfC_PINSeq,
    preconfLockingSeq,
    preconfACESeq,
};


} // namespace opal


std::string_view OpalModule::ModuleName() const {
    return "Opal";
}


eModuleKind OpalModule::ModuleKind() const {
    return eModuleKind::SSC;
}


std::optional<std::string> OpalModule::FindName(Uid uid) const {
    using namespace opal;
    static const auto lut = MakeNameLookup(allNames);

    const auto it = lut.find(uid);
    if (it != lut.end()) {
        return std::string(it->second);
    }

    for (const auto& seq : allNameSequences | std::views::join) {
        const auto result = FindNameSequence(uid, seq);
        if (result) {
            return *result;
        }
    }

    return std::nullopt;
}


std::optional<Uid> OpalModule::FindUid(std::string_view name) const {
    using namespace opal;
    static const auto lut = MakeUidLookup(allNames);

    const auto it = lut.find(name);
    if (it != lut.end()) {
        return it->second;
    }

    for (const auto& seq : allNameSequences | std::views::join) {
        const auto result = FindUidSequence(name, seq);
        if (result) {
            return *result;
        }
    }

    return std::nullopt;
}


std::optional<TableDesc> OpalModule::FindTable(Uid table) const {
    // TODO: TPerInfo has an additional column.
    return std::nullopt;
}


std::optional<Type> OpalModule::FindType(Uid uid) const {
    return std::nullopt;
}


std::span<const std::shared_ptr<Module>> OpalModule::Features() const {
    static const std::vector<std::shared_ptr<Module>> features = {
        std::make_shared<PSIDModule>(),
    };
    return features;
}