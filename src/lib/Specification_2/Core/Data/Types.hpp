#pragma once

#include "UIDs.hpp"

#include <Data/Type.hpp>

#include <cstdint>
#include <limits>
#include <vector>


namespace core {

inline const Type unknown_type = IdentifiedType<Type, 0x0000'0005'0000'0000>();

inline const Type boolean = IdentifiedType<EnumerationType, 0x0000'0005'0000'0401>(0, 1);

inline const Type integer_1 = IdentifiedType<SignedIntType, 0x0000'0005'0000'0210>(1);
inline const Type integer_2 = IdentifiedType<SignedIntType, 0x0000'0005'0000'0215>(2);
inline const Type uinteger_1 = IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0211>(1);
inline const Type uinteger_2 = IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0215>(2);
inline const Type uinteger_4 = IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0220>(4);
inline const Type uinteger_8 = IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0225>(8);

inline const Type max_bytes_32 = IdentifiedType<CappedBytesType, 0x0000'0005'0000'020D>(32);
inline const Type max_bytes_64 = IdentifiedType<CappedBytesType, 0x0000'0005'0000'020D>(64);
inline const Type bytes_12 = IdentifiedType<FixedBytesType, 0x0000'0005'0000'0201>(12);

inline const Type name = IdentifiedType<CappedBytesType, 0x0000'0005'0000'020B>(32);
inline const Type password = IdentifiedType<CappedBytesType, 0x0000'0005'0000'020C>(32);
inline const Type year_enum = IdentifiedType<EnumerationType, 0x0000'0005'0000'0416>(1970, 9999);
inline const Type month_enum = IdentifiedType<EnumerationType, 0x0000'0005'0000'0417>(1, 12);
inline const Type day_enum = IdentifiedType<EnumerationType, 0x0000'0005'0000'0418>(1, 31);
inline const Type Year = IdentifiedType<NameValueUintegerType, 0x0000'0005'0000'1401>(0, year_enum);
inline const Type Month = IdentifiedType<NameValueUintegerType, 0x0000'0005'0000'1402>(1, month_enum);
inline const Type Day = IdentifiedType<NameValueUintegerType, 0x0000'0005'0000'1403>(2, day_enum);
inline const Type date = IdentifiedType<StructType, 0x0000'0005'0000'1804>(Year, Month, Day);

inline const Type messaging_type = IdentifiedType<EnumerationType, 0x0000'0005'0000'0404>(0, 255);
inline const Type hash_protocol = IdentifiedType<EnumerationType, 0x0000'0005'0000'040D>(0, 15);
inline const Type auth_method = IdentifiedType<EnumerationType, 0x0000'0005'0000'0408>(0, 23);
inline const Type log_select = IdentifiedType<EnumerationType, 0x0000'0005'0000'040C>(0, 3);
inline const Type protect_types = IdentifiedType<SetType, 0x0000'0005'0000'1A05>(0, 255);
inline const Type reencrypt_request = IdentifiedType<EnumerationType, 0x0000'0005'0000'0413>(1, 16);
inline const Type reencrypt_state = IdentifiedType<EnumerationType, 0x0000'0005'0000'0414>(1, 16);
inline const Type reset_types = IdentifiedType<SetType, 0x0000'0005'0000'1A01>(1, 31);
inline const Type adv_key_mode = IdentifiedType<EnumerationType, 0x0000'0005'0000'040F>(0, 7);
inline const Type verify_mode = IdentifiedType<EnumerationType, 0x0000'0005'0000'0412>(0, 7);
inline const Type last_reenc_stat = IdentifiedType<EnumerationType, 0x0000'0005'0000'0411>(0, 7);
inline const Type gen_status = IdentifiedType<SetType, 0x0000'0005'0000'1A02>(0, 63);
inline const Type enc_supported = IdentifiedType<EnumerationType, 0x0000'0005'0000'041D>(0, 15);
inline const Type keys_avail_conds = IdentifiedType<EnumerationType, 0x0000'0005'0000'0410>(0, 7);

inline const Type uid = IdentifiedType<FixedBytesType, 0x0000'0005'0000'0209>(8);

inline const Type table_kind = IdentifiedType<EnumerationType, 0x0000'0005'0000'0415>(1, 8);
inline const Type object_ref = IdentifiedType<GeneralObjectReferenceType, 0x0000'0005'0000'0F02>();
inline const Type table_ref = IdentifiedType<GeneralTableReferenceType, 0x0000'0005'0000'0F03>();
inline const Type byte_table_ref = IdentifiedType<GeneralByteTableReferenceType, 0x0000'0005'0000'1001>();
inline const Type table_or_object_ref = IdentifiedType<AlternativeType, 0x0000'0005'0000'0606>(object_ref, table_ref);

inline const Type Authority_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C05>(uint64_t(eTable::Authority));
inline const Type Table_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C09>(uint64_t(eTable::Table));
inline const Type MethodID_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C03>(uint64_t(eTable::MethodID));
inline const Type SPTemplates_object = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C01>(uint64_t(eTable::SPTemplates));
inline const Type Column_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C07>(uint64_t(eTable::Column));
inline const Type Template_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C08>(uint64_t(eTable::Template));
inline const Type LogList_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C0D>(uint64_t(eTable::LogList));
inline const Type cred_object_uidref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C0B>(std::vector{
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
inline const Type mediakey_object_uidref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C0C>(std::vector{
    uint64_t(eTable::K_AES_128),
    uint64_t(eTable::K_AES_256),
});

inline const Type boolean_ACE = IdentifiedType<EnumerationType, 0x0000'0005'0000'0401>(0, 2);
inline const Type ACE_expression = IdentifiedType<AlternativeType, 0x0000'0005'0000'0601>(Authority_object_ref, boolean_ACE);
inline const Type AC_element = IdentifiedType<ListType, 0x0000'0005'0000'0801>(ACE_expression);
inline const Type ACE_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C04>(uint64_t(eTable::ACE));
inline const Type ACL = IdentifiedType<ListType, 0x0000'0005'0000'0802>(ACE_object_ref);
inline const Type ACE_columns = IdentifiedType<SetType, 0x0000'0005'0000'1A03>(0, std::numeric_limits<uint16_t>::max());

inline const Type life_cycle_state = IdentifiedType<EnumerationType, 0x0000'0005'0000'0405>(0, 15);
inline const Type SSC = IdentifiedType<ListType, 0x0000'0005'0000'0803>(name);

} // namespace core
