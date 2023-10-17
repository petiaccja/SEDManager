#include "ColumnTypes.hpp"

#include <unordered_map>


Type GetType(Uid id) {
    static const std::unordered_map<Uid, Type> registry = [] {
        using namespace column_types;

        std::unordered_map<Uid, Type> registry;
        const auto reg = [&registry](const Type& type) {
            registry.insert({ type_uid(type), type });
        };

        reg(unknown_type);
        reg(boolean);
        reg(integer_1);
        reg(integer_2);
        reg(uinteger_1);
        reg(uinteger_2);
        reg(uinteger_4);
        reg(uinteger_8);
        reg(max_bytes_32);
        reg(max_bytes_64);
        reg(bytes_12);
        reg(name);
        reg(password);
        reg(year_enum);
        reg(month_enum);
        reg(day_enum);
        reg(Year);
        reg(Month);
        reg(Day);
        reg(date);
        reg(messaging_type);
        reg(hash_protocol);
        reg(auth_method);
        reg(log_select);
        reg(protect_types);
        reg(reencrypt_request);
        reg(reencrypt_state);
        reg(reset_types);
        reg(adv_key_mode);
        reg(verify_mode);
        reg(last_reenc_stat);
        reg(gen_status);
        reg(enc_supported);
        reg(keys_avail_conds);
        reg(uid);
        reg(table_kind);
        reg(object_ref);
        reg(table_ref);
        reg(byte_table_ref);
        reg(table_or_object_ref);
        reg(Authority_object_ref);
        reg(Table_object_ref);
        reg(MethodID_object_ref);
        reg(SPTemplates_object);
        reg(Column_object_ref);
        reg(Template_object_ref);
        reg(LogList_object_ref);
        reg(cred_object_uidref);
        reg(mediakey_object_uidref);
        reg(boolean_ACE);
        reg(ACE_expression);
        reg(AC_element);
        reg(ACE_object_ref);
        reg(ACL);
        reg(ACE_columns);
        reg(life_cycle_state);
        reg(SSC);

        return registry;
    }();

    const auto it = registry.find(id);
    if (it != registry.end()) {
        throw std::invalid_argument(std::format("cannot find column type by its ID ({})", uint64_t(id)));
    }

    return it->second;
}