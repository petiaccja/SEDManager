#pragma once

#include "Identifiers.hpp"
#include "Type.hpp"

#include <limits>


namespace column_types {

const Type unknown_type = IdentifiedType<Type, 0x0000'0005'0000'0000>();

const Type boolean = IdentifiedType<EnumerationType, 0x0000'0005'0000'0401>(0, 1);

const Type integer_1 = IdentifiedType<SignedIntType, 0x0000'0005'0000'0210>(1);
const Type integer_2 = IdentifiedType<SignedIntType, 0x0000'0005'0000'0215>(2);
const Type uinteger_1 = IdentifiedType<SignedIntType, 0x0000'0005'0000'0211>(1);
const Type uinteger_2 = IdentifiedType<SignedIntType, 0x0000'0005'0000'0215>(2);
const Type uinteger_4 = IdentifiedType<SignedIntType, 0x0000'0005'0000'0220>(4);
const Type uinteger_8 = IdentifiedType<SignedIntType, 0x0000'0005'0000'0225>(8);

const Type max_bytes_32 = IdentifiedType<CappedBytesType, 0x0000'0005'0000'020D>(32);
const Type max_bytes_64 = IdentifiedType<CappedBytesType, 0x0000'0005'0000'020D>(64);
const Type bytes_12 = IdentifiedType<FixedBytesType, 0x0000'0005'0000'0201>(12);

const Type name = IdentifiedType<CappedBytesType, 0x0000'0005'0000'020B>(32);
const Type password = IdentifiedType<CappedBytesType, 0x0000'0005'0000'020C>(32);
const Type year_enum = IdentifiedType<EnumerationType, 0x0000'0005'0000'0416>(1970, 9999);
const Type month_enum = IdentifiedType<EnumerationType, 0x0000'0005'0000'0417>(1, 12);
const Type day_enum = IdentifiedType<EnumerationType, 0x0000'0005'0000'0418>(1, 31);
const Type Year = IdentifiedType<NameValueUintegerType, 0x0000'0005'0000'1401>(0, year_enum);
const Type Month = IdentifiedType<NameValueUintegerType, 0x0000'0005'0000'1402>(1, month_enum);
const Type Day = IdentifiedType<NameValueUintegerType, 0x0000'0005'0000'1403>(2, day_enum);
const Type date = IdentifiedType<StructType, 0x0000'0005'0000'1804>(Year, Month, Day);

const Type messaging_type = IdentifiedType<EnumerationType, 0x0000'0005'0000'0404>(0, 255);
const Type hash_protocol = IdentifiedType<EnumerationType, 0x0000'0005'0000'040D>(0, 15);
const Type auth_method = IdentifiedType<EnumerationType, 0x0000'0005'0000'0408>(0, 23);
const Type log_select = IdentifiedType<EnumerationType, 0x0000'0005'0000'040C>(0, 3);
const Type protect_types = IdentifiedType<SetType, 0x0000'0005'0000'1A05>(0, 255);
const Type reencrypt_request = IdentifiedType<EnumerationType, 0x0000'0005'0000'0413>(1, 16);
const Type reencrypt_state = IdentifiedType<EnumerationType, 0x0000'0005'0000'0414>(1, 16);
const Type reset_types = IdentifiedType<SetType, 0x0000'0005'0000'1A01>(1, 31);
const Type adv_key_mode = IdentifiedType<EnumerationType, 0x0000'0005'0000'040F>(0, 7);
const Type verify_mode = IdentifiedType<EnumerationType, 0x0000'0005'0000'0412>(0, 7);
const Type last_reenc_stat = IdentifiedType<EnumerationType, 0x0000'0005'0000'0411>(0, 7);
const Type gen_status = IdentifiedType<SetType, 0x0000'0005'0000'1A02>(0, 63);
const Type enc_supported = IdentifiedType<EnumerationType, 0x0000'0005'0000'041D>(0, 15);
const Type keys_avail_conds = IdentifiedType<EnumerationType, 0x0000'0005'0000'0410>(0, 7);

const Type uid = IdentifiedType<FixedBytesType, 0x0000'0005'0000'0209>(8);

const Type table_kind = IdentifiedType<EnumerationType, 0x0000'0005'0000'0415>(1, 8);
const Type object_ref = IdentifiedType<GeneralObjectReferenceType, 0x0000'0005'0000'0F02>();
const Type table_ref = IdentifiedType<GeneralTableReferenceType, 0x0000'0005'0000'0F03>();
const Type byte_table_ref = IdentifiedType<GeneralByteTableReferenceType, 0x0000'0005'0000'1001>();
const Type table_or_object_ref = IdentifiedType<AlternativeType, 0x0000'0005'0000'0606>(object_ref, table_ref);

const Type Authority_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'C005>(uint64_t(eTable::Authority));
const Type Table_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C09>(uint64_t(eTable::Table));
const Type MethodID_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C03>(uint64_t(eTable::MethodID));
const Type SPTemplates_object = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C01>(uint64_t(eTable::SPTemplates));
const Type Column_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C07>(uint64_t(eTable::Column));
const Type Template_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C08>(uint64_t(eTable::Template));
const Type LogList_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C0D>(uint64_t(eTable::LogList));
const Type cred_object_uidref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C0B>(std::vector{
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
const Type mediakey_object_uidref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C0C>(std::vector{
    uint64_t(eTable::K_AES_128),
    uint64_t(eTable::K_AES_256),
});

const Type boolean_ACE = IdentifiedType<EnumerationType, 0x0000'0005'0000'0401>(0, 2);
const Type ACE_expression = IdentifiedType<AlternativeType, 0x0000'0005'0000'0601>(Authority_object_ref, boolean_ACE);
const Type AC_element = IdentifiedType<ListType, 0x0000'0005'0000'0801>(ACE_expression);
const Type ACE_object_ref = IdentifiedType<RestrictedObjectReferenceType, 0x0000'0005'0000'0C04>(uint64_t(eTable::ACE));
const Type ACL = IdentifiedType<ListType, 0x0000'0005'0000'0802>(ACE_object_ref);
const Type ACE_columns = IdentifiedType<SetType, 0x0000'0005'0000'1A03>(0, std::numeric_limits<uint16_t>::max());

const Type life_cycle_state = IdentifiedType<EnumerationType, 0x0000'0005'0000'0405>(0, 15);
const Type SSC = IdentifiedType<ListType, 0x0000'0005'0000'0803>(name);

} // namespace column_types