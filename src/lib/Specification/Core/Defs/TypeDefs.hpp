#include "../../Common/Utility.hpp"
#include "UIDs.hpp"

#include <array>
#include <span>


namespace sedmgr {

namespace core {

    inline const std::initializer_list<std::pair<UID, Type>>& TypeDefs() {
        static const Type unknown_type = IdentifiedType<Type, uint64_t(eType::unknown_type)>();
        static const Type boolean = IdentifiedType<EnumerationType, uint64_t(eType::boolean)>(0, 1);

        static const Type integer_1 = IdentifiedType<SignedIntType, uint64_t(eType::integer_1)>(1);
        static const Type integer_2 = IdentifiedType<SignedIntType, uint64_t(eType::integer_2)>(2);
        static const Type uinteger_1 = IdentifiedType<UnsignedIntType, uint64_t(eType::uinteger_1)>(1);
        static const Type uinteger_2 = IdentifiedType<UnsignedIntType, uint64_t(eType::uinteger_2)>(2);
        static const Type uinteger_4 = IdentifiedType<UnsignedIntType, uint64_t(eType::uinteger_4)>(4);
        static const Type uinteger_8 = IdentifiedType<UnsignedIntType, uint64_t(eType::uinteger_8)>(8);

        static const Type max_bytes_32 = IdentifiedType<CappedBytesType, uint64_t(eType::max_bytes_32)>(32);
        static const Type max_bytes_64 = IdentifiedType<CappedBytesType, uint64_t(eType::max_bytes_64)>(64);
        static const Type bytes_4 = IdentifiedType<FixedBytesType, uint64_t(eType::bytes_4)>(4);
        static const Type bytes_12 = IdentifiedType<FixedBytesType, uint64_t(eType::bytes_12)>(12);
        static const Type bytes_16 = IdentifiedType<FixedBytesType, uint64_t(eType::bytes_16)>(16);
        static const Type bytes_32 = IdentifiedType<FixedBytesType, uint64_t(eType::bytes_32)>(32);
        static const Type bytes_64 = IdentifiedType<FixedBytesType, uint64_t(eType::bytes_64)>(64);

        static const Type key_128 = IdentifiedType<AlternativeType, uint64_t(eType::key_128)>(bytes_16, bytes_32);
        static const Type key_256 = IdentifiedType<AlternativeType, uint64_t(eType::key_256)>(bytes_32, bytes_64);

        static const Type type_def = IdentifiedType<CappedBytesType, uint64_t(eType::type_def)>(65536);
        static const Type name = IdentifiedType<CappedBytesType, uint64_t(eType::name)>(32);
        static const Type password = IdentifiedType<CappedBytesType, uint64_t(eType::password)>(32);
        static const Type year_enum = IdentifiedType<EnumerationType, uint64_t(eType::year_enum)>(1970, 9999);
        static const Type month_enum = IdentifiedType<EnumerationType, uint64_t(eType::month_enum)>(1, 12);
        static const Type day_enum = IdentifiedType<EnumerationType, uint64_t(eType::day_enum)>(1, 31);
        static const Type Year = IdentifiedType<NameValueUintegerType, uint64_t(eType::Year)>(0, year_enum);
        static const Type Month = IdentifiedType<NameValueUintegerType, uint64_t(eType::Month)>(1, month_enum);
        static const Type Day = IdentifiedType<NameValueUintegerType, uint64_t(eType::Day)>(2, day_enum);
        static const Type date = IdentifiedType<StructType, uint64_t(eType::date)>(Year, Month, Day);

        static const Type messaging_type = IdentifiedType<EnumerationType, uint64_t(eType::messaging_type)>(0, 255);
        static const Type hash_protocol = IdentifiedType<EnumerationType, uint64_t(eType::hash_protocol)>(0, 15);
        static const Type auth_method = IdentifiedType<EnumerationType, uint64_t(eType::auth_method)>(0, 23);
        static const Type log_select = IdentifiedType<EnumerationType, uint64_t(eType::log_select)>(0, 3);
        static const Type protect_types = IdentifiedType<SetType, uint64_t(eType::protect_types)>(0, 255);
        static const Type reencrypt_request = IdentifiedType<EnumerationType, uint64_t(eType::reencrypt_request)>(1, 16);
        static const Type reencrypt_state = IdentifiedType<EnumerationType, uint64_t(eType::reencrypt_state)>(1, 16);
        static const Type reset_types = IdentifiedType<SetType, uint64_t(eType::reset_types)>(1, 31);
        static const Type adv_key_mode = IdentifiedType<EnumerationType, uint64_t(eType::adv_key_mode)>(0, 7);
        static const Type verify_mode = IdentifiedType<EnumerationType, uint64_t(eType::verify_mode)>(0, 7);
        static const Type last_reenc_stat = IdentifiedType<EnumerationType, uint64_t(eType::last_reenc_stat)>(0, 7);
        static const Type gen_status = IdentifiedType<SetType, uint64_t(eType::gen_status)>(0, 63);
        static const Type enc_supported = IdentifiedType<EnumerationType, uint64_t(eType::enc_supported)>(0, 15);
        static const Type keys_avail_conds = IdentifiedType<EnumerationType, uint64_t(eType::keys_avail_conds)>(0, 7);
        static const Type symmetric_mode_media = IdentifiedType<EnumerationType, uint64_t(eType::symmetric_mode_media)>(0, 23);

        static const Type uid = IdentifiedType<FixedBytesType, uint64_t(eType::uid)>(8);

        static const Type table_kind = IdentifiedType<EnumerationType, uint64_t(eType::table_kind)>(1, 8);
        static const Type object_ref = IdentifiedType<GeneralObjectReferenceType, uint64_t(eType::object_ref)>();
        static const Type table_ref = IdentifiedType<GeneralTableReferenceType, uint64_t(eType::table_ref)>();
        static const Type byte_table_ref = IdentifiedType<GeneralByteTableReferenceType, uint64_t(eType::byte_table_ref)>();
        static const Type table_or_object_ref = IdentifiedType<AlternativeType, uint64_t(eType::table_or_object_ref)>(object_ref, table_ref);

        static const Type Authority_object_ref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::Authority_object_ref)>(uint64_t(eTable::Authority));
        static const Type Table_object_ref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::Table_object_ref)>(uint64_t(eTable::Table));
        static const Type MethodID_object_ref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::MethodID_object_ref)>(uint64_t(eTable::MethodID));
        static const Type SPTemplates_object = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::SPTemplates_object)>(uint64_t(eTable::SPTemplates));
        static const Type Column_object_ref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::Column_object_ref)>(uint64_t(eTable::Column));
        static const Type Template_object_ref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::Template_object_ref)>(uint64_t(eTable::Template));
        static const Type LogList_object_ref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::LogList_object_ref)>(uint64_t(eTable::LogList));
        static const Type cred_object_uidref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::cred_object_uidref)>(std::vector{
            uint64_t(eTable::C_PIN),
            uint64_t(eTable::C_AES_128),
            uint64_t(eTable::C_AES_256),
            uint64_t(eTable::C_RSA_1024),
            uint64_t(eTable::C_RSA_2048),
            uint64_t(eTable::C_EC_160),
            uint64_t(eTable::C_EC_192),
            uint64_t(eTable::C_EC_224),
            uint64_t(eTable::C_EC_256),
            uint64_t(eTable::C_EC_384),
            uint64_t(eTable::C_EC_521),
            uint64_t(eTable::C_EC_163),
            uint64_t(eTable::C_EC_233),
            uint64_t(eTable::C_EC_283),
            uint64_t(eTable::C_HMAC_160),
            uint64_t(eTable::C_HMAC_256),
            uint64_t(eTable::C_HMAC_384),
            uint64_t(eTable::C_HMAC_512),
        });
        static const Type mediakey_object_uidref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::mediakey_object_uidref)>(std::vector{
            uint64_t(eTable::K_AES_128),
            uint64_t(eTable::K_AES_256),
        });

        static const Type boolean_ACE = IdentifiedType<EnumerationType, uint64_t(eType::boolean_ACE)>(0, 2, std::initializer_list<std::pair<uint16_t, std::string_view>>{
                                                                                                                {0,  "And"},
                                                                                                                { 1, "Or" },
                                                                                                                { 2, "Not"}
        });
        static const Type ACE_expression = IdentifiedType<AlternativeType, uint64_t(eType::ACE_expression)>(Authority_object_ref, boolean_ACE);
        static const Type AC_element = IdentifiedType<ListType, uint64_t(eType::AC_element)>(ACE_expression);
        static const Type ACE_object_ref = IdentifiedType<RestrictedObjectReferenceType, uint64_t(eType::ACE_object_ref)>(uint64_t(eTable::ACE));
        static const Type ACL = IdentifiedType<ListType, uint64_t(eType::ACL)>(ACE_object_ref);
        static const Type ACE_columns = IdentifiedType<SetType, uint64_t(eType::ACE_columns)>(0, std::numeric_limits<uint16_t>::max());

        static const Type life_cycle_state = IdentifiedType<EnumerationType, uint64_t(eType::life_cycle_state)>(0, 15);
        static const Type SSC = IdentifiedType<ListType, uint64_t(eType::SSC)>(name);

        static const std::initializer_list<std::pair<UID, Type>> typeDefs = {
            {eType::unknown_type,            unknown_type          },
            { eType::boolean,                boolean               },
            { eType::integer_1,              integer_1             },
            { eType::integer_2,              integer_2             },
            { eType::uinteger_1,             uinteger_1            },
            { eType::uinteger_2,             uinteger_2            },
            { eType::uinteger_4,             uinteger_4            },
            { eType::uinteger_8,             uinteger_8            },
            { eType::max_bytes_32,           max_bytes_32          },
            { eType::max_bytes_64,           max_bytes_64          },
            { eType::bytes_4,                bytes_4               },
            { eType::bytes_12,               bytes_12              },
            { eType::bytes_16,               bytes_16              },
            { eType::bytes_32,               bytes_32              },
            { eType::bytes_64,               bytes_64              },
            { eType::key_128,                key_128               },
            { eType::key_256,                key_256               },
            { eType::type_def,               type_def              },
            { eType::name,                   name                  },
            { eType::password,               password              },
            { eType::year_enum,              year_enum             },
            { eType::month_enum,             month_enum            },
            { eType::day_enum,               day_enum              },
            { eType::Year,                   Year                  },
            { eType::Month,                  Month                 },
            { eType::Day,                    Day                   },
            { eType::date,                   date                  },
            { eType::messaging_type,         messaging_type        },
            { eType::hash_protocol,          hash_protocol         },
            { eType::auth_method,            auth_method           },
            { eType::log_select,             log_select            },
            { eType::protect_types,          protect_types         },
            { eType::reencrypt_request,      reencrypt_request     },
            { eType::reencrypt_state,        reencrypt_state       },
            { eType::reset_types,            reset_types           },
            { eType::adv_key_mode,           adv_key_mode          },
            { eType::verify_mode,            verify_mode           },
            { eType::last_reenc_stat,        last_reenc_stat       },
            { eType::gen_status,             gen_status            },
            { eType::enc_supported,          enc_supported         },
            { eType::keys_avail_conds,       keys_avail_conds      },
            { eType::symmetric_mode_media,   symmetric_mode_media  },
            { eType::uid,                    uid                   },
            { eType::table_kind,             table_kind            },
            { eType::object_ref,             object_ref            },
            { eType::table_ref,              table_ref             },
            { eType::byte_table_ref,         byte_table_ref        },
            { eType::table_or_object_ref,    table_or_object_ref   },
            { eType::Authority_object_ref,   Authority_object_ref  },
            { eType::Table_object_ref,       Table_object_ref      },
            { eType::MethodID_object_ref,    MethodID_object_ref   },
            { eType::SPTemplates_object,     SPTemplates_object    },
            { eType::Column_object_ref,      Column_object_ref     },
            { eType::Template_object_ref,    Template_object_ref   },
            { eType::LogList_object_ref,     LogList_object_ref    },
            { eType::cred_object_uidref,     cred_object_uidref    },
            { eType::mediakey_object_uidref, mediakey_object_uidref},
            { eType::boolean_ACE,            boolean_ACE           },
            { eType::ACE_expression,         ACE_expression        },
            { eType::AC_element,             AC_element            },
            { eType::ACE_object_ref,         ACE_object_ref        },
            { eType::ACL,                    ACL                   },
            { eType::ACE_columns,            ACE_columns           },
            { eType::life_cycle_state,       life_cycle_state      },
            { eType::SSC,                    SSC                   },
        };
        return typeDefs;
    };

} // namespace core

} // namespace sedmgr