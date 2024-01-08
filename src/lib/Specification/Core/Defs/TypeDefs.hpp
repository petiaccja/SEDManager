#include "../../Common/Utility.hpp"
#include "UIDs.hpp"

#include <array>
#include <span>


namespace sedmgr {

namespace core {

    inline const std::initializer_list<std::pair<UID, Type>>& TypeDefs() {
        static const Type unknown_type = IdentifiedType<Type, UID(eType::unknown_type)>();
        static const Type boolean = IdentifiedType<EnumerationType, UID(eType::boolean)>(0, 1);

        static const Type integer_1 = IdentifiedType<SignedIntType, UID(eType::integer_1)>(1);
        static const Type integer_2 = IdentifiedType<SignedIntType, UID(eType::integer_2)>(2);
        static const Type uinteger_1 = IdentifiedType<UnsignedIntType, UID(eType::uinteger_1)>(1);
        static const Type uinteger_2 = IdentifiedType<UnsignedIntType, UID(eType::uinteger_2)>(2);
        static const Type uinteger_4 = IdentifiedType<UnsignedIntType, UID(eType::uinteger_4)>(4);
        static const Type uinteger_8 = IdentifiedType<UnsignedIntType, UID(eType::uinteger_8)>(8);

        static const Type max_bytes_32 = IdentifiedType<CappedBytesType, UID(eType::max_bytes_32)>(32);
        static const Type max_bytes_64 = IdentifiedType<CappedBytesType, UID(eType::max_bytes_64)>(64);
        static const Type bytes_4 = IdentifiedType<FixedBytesType, UID(eType::bytes_4)>(4);
        static const Type bytes_12 = IdentifiedType<FixedBytesType, UID(eType::bytes_12)>(12);
        static const Type bytes_16 = IdentifiedType<FixedBytesType, UID(eType::bytes_16)>(16);
        static const Type bytes_32 = IdentifiedType<FixedBytesType, UID(eType::bytes_32)>(32);
        static const Type bytes_64 = IdentifiedType<FixedBytesType, UID(eType::bytes_64)>(64);

        static const Type key_128 = IdentifiedType<AlternativeType, UID(eType::key_128)>(bytes_16, bytes_32);
        static const Type key_256 = IdentifiedType<AlternativeType, UID(eType::key_256)>(bytes_32, bytes_64);

        static const Type type_def = IdentifiedType<CappedBytesType, UID(eType::type_def)>(65536);
        static const Type name = IdentifiedType<CappedBytesType, UID(eType::name)>(32);
        static const Type password = IdentifiedType<CappedBytesType, UID(eType::password)>(32);
        static const Type year_enum = IdentifiedType<EnumerationType, UID(eType::year_enum)>(1970, 9999);
        static const Type month_enum = IdentifiedType<EnumerationType, UID(eType::month_enum)>(1, 12);
        static const Type day_enum = IdentifiedType<EnumerationType, UID(eType::day_enum)>(1, 31);
        static const Type Year = IdentifiedType<NameValueUintegerType, UID(eType::Year)>(0, year_enum);
        static const Type Month = IdentifiedType<NameValueUintegerType, UID(eType::Month)>(1, month_enum);
        static const Type Day = IdentifiedType<NameValueUintegerType, UID(eType::Day)>(2, day_enum);
        static const Type date = IdentifiedType<StructType, UID(eType::date)>(Year, Month, Day);

        static const Type messaging_type = IdentifiedType<EnumerationType, UID(eType::messaging_type)>(0, 255);
        static const Type hash_protocol = IdentifiedType<EnumerationType, UID(eType::hash_protocol)>(0, 15);
        static const Type auth_method = IdentifiedType<EnumerationType, UID(eType::auth_method)>(0, 23);
        static const Type log_select = IdentifiedType<EnumerationType, UID(eType::log_select)>(0, 3);
        static const Type protect_types = IdentifiedType<SetType, UID(eType::protect_types)>(0, 255);
        static const Type reencrypt_request = IdentifiedType<EnumerationType, UID(eType::reencrypt_request)>(1, 16);
        static const Type reencrypt_state = IdentifiedType<EnumerationType, UID(eType::reencrypt_state)>(1, 16);
        static const Type reset_types = IdentifiedType<SetType, UID(eType::reset_types)>(1, 31);
        static const Type adv_key_mode = IdentifiedType<EnumerationType, UID(eType::adv_key_mode)>(0, 7);
        static const Type verify_mode = IdentifiedType<EnumerationType, UID(eType::verify_mode)>(0, 7);
        static const Type last_reenc_stat = IdentifiedType<EnumerationType, UID(eType::last_reenc_stat)>(0, 7);
        static const Type gen_status = IdentifiedType<SetType, UID(eType::gen_status)>(0, 63);
        static const Type enc_supported = IdentifiedType<EnumerationType, UID(eType::enc_supported)>(0, 15);
        static const Type keys_avail_conds = IdentifiedType<EnumerationType, UID(eType::keys_avail_conds)>(0, 7);
        static const Type symmetric_mode_media = IdentifiedType<EnumerationType, UID(eType::symmetric_mode_media)>(0, 23);

        static const Type uid = IdentifiedType<FixedBytesType, UID(eType::uid)>(8);

        static const Type table_kind = IdentifiedType<EnumerationType, UID(eType::table_kind)>(1, 8);
        static const Type object_ref = IdentifiedType<GeneralObjectReferenceType, UID(eType::object_ref)>();
        static const Type table_ref = IdentifiedType<GeneralTableReferenceType, UID(eType::table_ref)>();
        static const Type byte_table_ref = IdentifiedType<GeneralByteTableReferenceType, UID(eType::byte_table_ref)>();
        static const Type table_or_object_ref = IdentifiedType<AlternativeType, UID(eType::table_or_object_ref)>(object_ref, table_ref);

        static const Type Authority_object_ref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::Authority_object_ref)>(UID(eTable::Authority));
        static const Type Table_object_ref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::Table_object_ref)>(UID(eTable::Table));
        static const Type MethodID_object_ref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::MethodID_object_ref)>(UID(eTable::MethodID));
        static const Type SPTemplates_object = IdentifiedType<RestrictedObjectReferenceType, UID(eType::SPTemplates_object)>(UID(eTable::SPTemplates));
        static const Type Column_object_ref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::Column_object_ref)>(UID(eTable::Column));
        static const Type Template_object_ref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::Template_object_ref)>(UID(eTable::Template));
        static const Type LogList_object_ref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::LogList_object_ref)>(UID(eTable::LogList));
        static const Type cred_object_uidref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::cred_object_uidref)>(std::vector{
            UID(eTable::C_PIN),
            UID(eTable::C_AES_128),
            UID(eTable::C_AES_256),
            UID(eTable::C_RSA_1024),
            UID(eTable::C_RSA_2048),
            UID(eTable::C_EC_160),
            UID(eTable::C_EC_192),
            UID(eTable::C_EC_224),
            UID(eTable::C_EC_256),
            UID(eTable::C_EC_384),
            UID(eTable::C_EC_521),
            UID(eTable::C_EC_163),
            UID(eTable::C_EC_233),
            UID(eTable::C_EC_283),
            UID(eTable::C_HMAC_160),
            UID(eTable::C_HMAC_256),
            UID(eTable::C_HMAC_384),
            UID(eTable::C_HMAC_512),
        });
        static const Type mediakey_object_uidref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::mediakey_object_uidref)>(std::vector{
            UID(eTable::K_AES_128),
            UID(eTable::K_AES_256),
        });

        static const Type boolean_ACE = IdentifiedType<EnumerationType, UID(eType::boolean_ACE)>(0, 2, std::initializer_list<std::pair<uint16_t, std::string_view>>{
                                                                                                           {0,  "And"},
                                                                                                           { 1, "Or" },
                                                                                                           { 2, "Not"}
        });
        static const Type ACE_expression = IdentifiedType<AlternativeType, UID(eType::ACE_expression)>(Authority_object_ref, boolean_ACE);
        static const Type AC_element = IdentifiedType<ListType, UID(eType::AC_element)>(ACE_expression);
        static const Type ACE_object_ref = IdentifiedType<RestrictedObjectReferenceType, UID(eType::ACE_object_ref)>(UID(eTable::ACE));
        static const Type ACL = IdentifiedType<ListType, UID(eType::ACL)>(ACE_object_ref);
        static const Type ACE_columns = IdentifiedType<SetType, UID(eType::ACE_columns)>(0, std::numeric_limits<uint16_t>::max());

        static const Type life_cycle_state = IdentifiedType<EnumerationType, UID(eType::life_cycle_state)>(0, 15);
        static const Type SSC = IdentifiedType<ListType, UID(eType::SSC)>(name);

        static const std::initializer_list<std::pair<UID, Type>> typeDefs = {
            {UID(eType::unknown_type),            unknown_type          },
            { UID(eType::boolean),                boolean               },
            { UID(eType::integer_1),              integer_1             },
            { UID(eType::integer_2),              integer_2             },
            { UID(eType::uinteger_1),             uinteger_1            },
            { UID(eType::uinteger_2),             uinteger_2            },
            { UID(eType::uinteger_4),             uinteger_4            },
            { UID(eType::uinteger_8),             uinteger_8            },
            { UID(eType::max_bytes_32),           max_bytes_32          },
            { UID(eType::max_bytes_64),           max_bytes_64          },
            { UID(eType::bytes_4),                bytes_4               },
            { UID(eType::bytes_12),               bytes_12              },
            { UID(eType::bytes_16),               bytes_16              },
            { UID(eType::bytes_32),               bytes_32              },
            { UID(eType::bytes_64),               bytes_64              },
            { UID(eType::key_128),                key_128               },
            { UID(eType::key_256),                key_256               },
            { UID(eType::type_def),               type_def              },
            { UID(eType::name),                   name                  },
            { UID(eType::password),               password              },
            { UID(eType::year_enum),              year_enum             },
            { UID(eType::month_enum),             month_enum            },
            { UID(eType::day_enum),               day_enum              },
            { UID(eType::Year),                   Year                  },
            { UID(eType::Month),                  Month                 },
            { UID(eType::Day),                    Day                   },
            { UID(eType::date),                   date                  },
            { UID(eType::messaging_type),         messaging_type        },
            { UID(eType::hash_protocol),          hash_protocol         },
            { UID(eType::auth_method),            auth_method           },
            { UID(eType::log_select),             log_select            },
            { UID(eType::protect_types),          protect_types         },
            { UID(eType::reencrypt_request),      reencrypt_request     },
            { UID(eType::reencrypt_state),        reencrypt_state       },
            { UID(eType::reset_types),            reset_types           },
            { UID(eType::adv_key_mode),           adv_key_mode          },
            { UID(eType::verify_mode),            verify_mode           },
            { UID(eType::last_reenc_stat),        last_reenc_stat       },
            { UID(eType::gen_status),             gen_status            },
            { UID(eType::enc_supported),          enc_supported         },
            { UID(eType::keys_avail_conds),       keys_avail_conds      },
            { UID(eType::symmetric_mode_media),   symmetric_mode_media  },
            { UID(eType::uid),                    uid                   },
            { UID(eType::table_kind),             table_kind            },
            { UID(eType::object_ref),             object_ref            },
            { UID(eType::table_ref),              table_ref             },
            { UID(eType::byte_table_ref),         byte_table_ref        },
            { UID(eType::table_or_object_ref),    table_or_object_ref   },
            { UID(eType::Authority_object_ref),   Authority_object_ref  },
            { UID(eType::Table_object_ref),       Table_object_ref      },
            { UID(eType::MethodID_object_ref),    MethodID_object_ref   },
            { UID(eType::SPTemplates_object),     SPTemplates_object    },
            { UID(eType::Column_object_ref),      Column_object_ref     },
            { UID(eType::Template_object_ref),    Template_object_ref   },
            { UID(eType::LogList_object_ref),     LogList_object_ref    },
            { UID(eType::cred_object_uidref),     cred_object_uidref    },
            { UID(eType::mediakey_object_uidref), mediakey_object_uidref},
            { UID(eType::boolean_ACE),            boolean_ACE           },
            { UID(eType::ACE_expression),         ACE_expression        },
            { UID(eType::AC_element),             AC_element            },
            { UID(eType::ACE_object_ref),         ACE_object_ref        },
            { UID(eType::ACL),                    ACL                   },
            { UID(eType::ACE_columns),            ACE_columns           },
            { UID(eType::life_cycle_state),       life_cycle_state      },
            { UID(eType::SSC),                    SSC                   },
        };
        return typeDefs;
    };

} // namespace core

} // namespace sedmgr