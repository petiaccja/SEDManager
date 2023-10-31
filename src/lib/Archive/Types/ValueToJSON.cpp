#include "ValueToJSON.hpp"

#include <Archive/Conversion.hpp>
#include <Error/Exception.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Core/Defs/Types.hpp>

#include <sstream>

// Hierarchy of types:
//
// IntegerType
// - SignedIntType
// - UnsignedIntType
// - EnumerationType
// BytesType
// - CappedBytesType
// - FixedBytesType
//
// AlternativeType
// ListType
// - SetType
// StructType
//
// RestrictedReferenceType
// - RestrictedByteReferenceType
// - RestrictedObjectReferenceType
//
// GeneralReferenceType
// - GeneralByteReferenceType
// - GeneralObjectReferenceType
//   - GeneralTableReferenceType
//   - GeneralByteTableReferenceType
//   - GeneralObjectTableReferenceType


namespace impl {
namespace {

    //--------------------------------------------------------------------------
    // Get string from type
    //--------------------------------------------------------------------------

    std::string GetTypeStr(const IntegerType& type) {
        return std::format("{}integer_{}", type.Signedness() ? "" : "u", type.Width());
    }

    std::string GetTypeStr(const BytesType& type) {
        return std::format("{}bytes_{}", type.Fixed() ? "" : "max_", type.Length());
    }

    std::string GetTypeStr(const AlternativeType& type) {
        std::stringstream ss;
        ss << "typeOr{ ";
        const auto& altTypes = type.Types();
        for (const auto& alt : altTypes) {
            try {
                ss << std::format("{:08X}:", uint32_t(uint64_t(type_uid(alt))));
            }
            catch (std::exception&) {
                // Not an identified type: ignore.
            }
            ss << GetTypeStr(alt);
            if (&alt != &altTypes.back()) {
                ss << " | ";
            }
        }
        ss << " }";
        return ss.str();
    }

    std::string GetTypeStr(const ListType& type) {
        return std::format("list{{ {} }}", GetTypeStr(type.ElementType()));
    }

    std::string GetTypeStr(const StructType& type) {
        std::stringstream ss;
        ss << "struct{ ";
        const auto& elementTypes = type.ElementTypes();
        for (const auto& alt : elementTypes) {
            ss << GetTypeStr(alt);
            if (&alt != &elementTypes.back()) {
                ss << ", ";
            }
        }
        ss << " }";
        return ss.str();
    }

    std::string GetTypeStr(const RestrictedReferenceType& type) {
        std::stringstream ss;
        ss << "{ ";
        const auto& tables = type.Tables();
        for (const auto& table : tables) {
            ss << CoreModule::Get()->FindName(table).value_or(to_string(table));
            if (&table != &tables.back()) {
                ss << " | ";
            }
        }
        ss << " }";

        if (type_isa<RestrictedObjectReferenceType>(type)) {
            return "objref" + ss.str();
        }
        else if (type_isa<RestrictedByteReferenceType>(type)) {
            return "byteref" + ss.str();
        }
        return "ref" + ss.str();
    }

    std::string GetTypeStr(const GeneralReferenceType& type) {
        if (type_isa<GeneralObjectReferenceType>(type)) {
            return "objref";
        }
        else if (type_isa<GeneralTableReferenceType>(type)) {
            return "tableref";
        }
        else if (type_isa<GeneralByteReferenceType>(type)) {
            return "byteref";
        }
        return "ref";
    }

    std::string GetTypeStr(const NameValueUintegerType& type) {
        return std::format("named{{ {}: {} }}", type.Name(), GetTypeStr(type.ValueType()));
    }

    //--------------------------------------------------------------------------
    // Get JSON from value
    //--------------------------------------------------------------------------

    bool InterpretAsString(const BytesType& type) {
        try {
            const auto uid = type_uid(type);
            if (uid == type_uid(core::name) || uid == type_uid(core::password)) {
                return true;
            }
        }
        catch (std::exception&) {
            return false;
        }
        return false;
    }

    nlohmann::json BytesToJSON(std::span<const std::byte> bytes, std::string_view prefix, bool asString) {
        std::string rep = std::string{ prefix };
        for (const auto& byte : bytes) {
            if (!asString) {
                rep += std::format("{:02X}", uint8_t(byte));
                if (&byte != &bytes.back()) {
                    rep += "'";
                }
            }
            else {
                rep += static_cast<char>(byte);
            }
        }
        return nlohmann::json(rep);
    }

    std::vector<std::byte> JSONToBytes(nlohmann::json json, std::string_view prefix, bool asString) {
        if (!json.is_string()) {
            throw UnexpectedTypeError("JSON:string");
        }
        auto str = json.get<std::string_view>();
        if (str.find(prefix) != 0) {
            throw InvalidFormatError(std::format("prefix \"{}\" not found", prefix));
        }
        str = str.substr(prefix.size());
        if (asString) {
            const auto bytes = std::as_bytes(std::span(str));
            return { bytes.begin(), bytes.end() };
        }
        else {
            std::vector<std::byte> bytes;
            size_t pos = 0;
            std::string chunk;
            while (pos != str.size()) {
                if (str[pos] == '\'') {
                    ++pos;
                    continue;
                }
                const auto start = pos;
                chunk.assign(str.substr(pos, 2));
                if (chunk.size() != 2) {
                    throw InvalidFormatError("bytes type must have an even number of hexadecimal digits");
                }
                size_t idx = 0;
                try {
                    bytes.push_back(static_cast<std::byte>(std::stoul(chunk, &idx, 16)));
                }
                catch (std::exception&) {
                    throw InvalidFormatError(std::format("invalid character in byte data: {}", chunk));
                }
                if (idx != 2) {
                    throw InvalidFormatError(std::format("invalid character in byte data: {}", chunk));
                }
                pos += 2;
            }
            return bytes;
        }
    }

    nlohmann::json ValueToJSON(const Value& value, const EnumerationType& type) {
        if (value.IsInteger()) {
            const auto enumValue = value.Get<uint16_t>();
            const auto& lookupByValue = type.ByValue();
            const auto it = lookupByValue.find(enumValue);
            if (it != lookupByValue.end()) {
                return nlohmann::json(it->second);
            }
            return nlohmann::json(enumValue);
        }
        throw UnexpectedTypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const IntegerType& type) {
        if (value.IsInteger()) {
            if (type.Signedness()) {
                return nlohmann::json(value.Get<int64_t>());
            }
            else {
                return nlohmann::json(value.Get<uint64_t>());
            }
        }
        throw UnexpectedTypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const BytesType& type) {
        if (value.IsBytes()) {
            return BytesToJSON(value.GetBytes(), "", InterpretAsString(type));
        }
        throw UnexpectedTypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const AlternativeType& type) {
        if (value.IsNamed()) {
            const auto& alts = type.Types();
            const auto currentTypeType = BytesType(4, true);
            const Value& currentType = value.GetNamed().name;
            const Value& currentValue = value.GetNamed().value;
            if (!currentType.IsBytes()) {
                throw UnexpectedTypeError(GetTypeStr(currentTypeType), value.GetTypeStr());
            }
            uint32_t altUidLower = 0;
            FromBytes(currentType.GetBytes(), altUidLower);
            const Uid altUid = uint64_t(altUidLower) | baseTypeUid;
            const auto altIter = std::ranges::find_if(alts, [&altUid](const Type& alt) {
                try {
                    return type_uid(alt) == altUid;
                }
                catch (std::bad_cast&) {
                    throw UnexpectedTypeError("<any identified type>", GetTypeStr(alt));
                }
            });
            if (altIter == alts.end()) {
                throw UnexpectedTypeError(GetTypeStr(type), "uid:" + to_string(altUid));
            }

            return nlohmann::json({
                {ValueToJSON(ToBytes(uint64_t(altUid)), ReferenceType{}), ValueToJSON(currentValue, *altIter)}
            });
        }
        throw UnexpectedTypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const ListType& type) {
        if (value.IsList()) {
            std::vector<nlohmann::json> jsons;
            const auto& items = value.GetList();
            for (auto& item : items) {
                jsons.push_back(ValueToJSON(item, type.ElementType()));
            }
            return nlohmann::json(jsons);
        }
        throw UnexpectedTypeError(GetTypeStr(type), value.GetTypeStr());
    }

    struct ReducedStruct {
        std::vector<Type> mandatoryFields;
        std::unordered_map<uint64_t, Type> optionalFields;
    };

    ReducedStruct ReduceStructType(const StructType& type) {
        ReducedStruct s;

        const auto& elementTypes = type.ElementTypes();
        auto mandatoryTypes = elementTypes | std::views::filter([](const Type& t) { return !type_isa<NameValueUintegerType>(t); });
        auto optionalTypes = elementTypes | std::views::filter([](const Type& t) { return type_isa<NameValueUintegerType>(t); });

        for (auto& type : mandatoryTypes) {
            s.mandatoryFields.push_back(type);
        }
        for (const auto& optionalType : optionalTypes) {
            const auto& namedType = type_cast<NameValueUintegerType>(optionalType);
            s.optionalFields.insert({ namedType.Name(), namedType.ValueType() });
        }
        return s;
    }

    nlohmann::json ValueToJSON(const Value& value, const StructType& type) {
        if (value.IsList()) {
            const auto& elements = value.GetList();

            const auto [mandatoryTypes, optionalTypeLookup] = ReduceStructType(type);
            auto mandatoryElements = elements | std::views::filter([](const Value& v) { return !v.IsNamed(); });
            auto optionalElements = elements | std::views::filter([](const Value& v) { return v.IsNamed(); });

            std::vector<nlohmann::json> jsons;

            auto [meit, mtit] = std::tuple(begin(mandatoryElements), begin(mandatoryTypes));
            for (; meit != end(mandatoryElements) && mtit != end(mandatoryTypes); ++meit, ++mtit) {
                jsons.emplace_back(ValueToJSON(*meit, *mtit));
            }
            if (meit != end(mandatoryElements) || mtit != end(mandatoryTypes)) {
                throw InvalidTypeError(std::format("value does not contain the same number of mandatory fields specified in type '{}'", GetTypeStr(type)));
            }

            for (const auto& optionalElement : optionalElements) {
                const auto& named = optionalElement.GetNamed();
                if (!named.name.IsInteger()) {
                    throw InvalidTypeError(std::format("optional element of struct type must have integer key: got '{}'", named.name.GetTypeStr()));
                }
                const auto key = named.name.Get<uint64_t>();
                const auto typeIt = optionalTypeLookup.find(key);
                if (typeIt == optionalTypeLookup.end()) {
                    throw InvalidTypeError(std::format("unexpected optional element in value of type struct: element key {}", key));
                }
                jsons.push_back({
                    { "field", key },
                    { "value", ValueToJSON(named.value, typeIt->second) },
                });
            }
            return nlohmann::json(jsons);
        }
        throw UnexpectedTypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const ReferenceType& type) {
        if (value.IsBytes()) {
            uint64_t uid;
            FromBytes(value.GetBytes(), uid);
            return "ref:" + to_string(Uid(uid));
        }
        throw UnexpectedTypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const NameValueUintegerType& type) {
        if (value.IsNamed()) {
            const auto& name = value.GetNamed().name;
            if (!name.IsInteger()) {
                throw UnexpectedTypeError("integer", name.GetTypeStr());
            }
            if (name.GetInt<uint16_t>() != type.Name()) {
                throw InvalidTypeError(std::format("name encoded in Value ({}) does not match name encoded in Type ({})", name.GetInt<uint16_t>(), type.Name()));
            }
            return nlohmann::json({
                { "name", type.Name() },
                { "value", ValueToJSON(value.GetNamed().value, type.ValueType()) },
            });
        }
        throw UnexpectedTypeError(GetTypeStr(type), value.GetTypeStr());
    }


    Value JSONToValue(const nlohmann::json& json, const EnumerationType& type) {
        if (json.is_string()) {
            const auto& lookupByName = type.ByName();
            auto it = lookupByName.find(json.get<std::string>());
            if (it != lookupByName.end()) {
                return Value(it->second);
            }
            throw InvalidFormatError(std::format("invalid enumeration value '{}'", json.get<std::string>()));
        }
        else if (json.is_number_integer()) {
            return Value(json.get<uint16_t>());
        }
        else {
            throw UnexpectedTypeError("{ string | int }", json.type_name());
        }
    }


    Value JSONToValue(const nlohmann::json& json, const IntegerType& type) {
        if (!json.is_number_integer()) {
            throw UnexpectedTypeError("integer");
        }
        if (type.Signedness()) {
            const auto number = json.get<int64_t>();
            switch (type.Width()) {
                case 1: return int8_t(number);
                case 2: return int16_t(number);
                case 4: return int32_t(number);
                default: return number;
            }
        }
        else {
            const auto number = json.get<uint64_t>();
            switch (type.Width()) {
                case 1: return uint8_t(number);
                case 2: return uint16_t(number);
                case 4: return uint32_t(number);
                default: return number;
            }
        }
    }

    Value JSONToValue(const nlohmann::json& json, const BytesType& type) {
        return JSONToBytes(json, "", InterpretAsString(type));
    }

    Value JSONToValue(const nlohmann::json& json, const AlternativeType& type) {
        if (!json.is_object() || json.empty()) {
            throw UnexpectedTypeError("object: { type: value }", json.type_name());
        }
        const auto& altJson = json.begin().key();
        const auto& valueJson = json.begin().value();
        uint64_t altUidBits = 0;
        FromBytes(JSONToValue(altJson, ReferenceType{}).GetBytes(), altUidBits);
        const Uid altUid = altUidBits;

        const auto& alts = type.Types();
        const auto altIter = std::ranges::find_if(alts, [&altUid](const Type& alt) {
            try {
                return type_uid(alt) == altUid;
            }
            catch (std::bad_cast&) {
                throw UnexpectedTypeError("<any identified type>", GetTypeStr(alt));
            }
        });
        if (altIter == alts.end()) {
            throw UnexpectedTypeError(GetTypeStr(type), "uid:" + to_string(altUid));
        }

        return Named{
            ToBytes(uint32_t(uint64_t(altUid))),
            JSONToValue(valueJson, *altIter),
        };
    }

    Value JSONToValue(const nlohmann::json& json, const ListType& type) {
        if (!json.is_array()) {
            throw UnexpectedTypeError("list");
        }
        std::vector<Value> values;
        for (const auto& item : json) {
            values.push_back(JSONToValue(item, type.ElementType()));
        }
        return values;
    }

    bool IsOptionalField(const nlohmann::json& json) {
        return json.contains("field") && json.contains("value");
    }

    Value JSONToValue(const nlohmann::json& json, const StructType& type) {
        if (json.is_array()) {
            const auto& elements = json;

            const auto [mandatoryTypes, optionalTypeLookup] = ReduceStructType(type);
            auto mandatoryElements = elements | std::views::filter([](const auto& v) { return !IsOptionalField(v); });
            auto optionalElements = elements | std::views::filter([](const auto& v) { return IsOptionalField(v); });

            std::vector<Value> values;

            auto [meit, mtit] = std::tuple(begin(mandatoryElements), begin(mandatoryTypes));
            for (; meit != end(mandatoryElements) && mtit != end(mandatoryTypes); ++meit, ++mtit) {
                values.emplace_back(JSONToValue(*meit, *mtit));
            }
            if (meit != end(mandatoryElements) || mtit != end(mandatoryTypes)) {
                throw InvalidTypeError(std::format("value does not contain the same number of mandatory fields specified in '{}'", GetTypeStr(type)));
            }

            for (const auto& optionalElement : optionalElements) {
                const auto& field = optionalElement["field"];
                const auto& value = optionalElement["value"];
                if (!field.is_number_integer()) {
                    throw InvalidTypeError(std::format("optional element of struct type must have integer key: got '{}'", field.dump()));
                }
                const auto key = field.get<uint64_t>();
                const auto typeIt = optionalTypeLookup.find(key);
                if (typeIt == optionalTypeLookup.end()) {
                    throw InvalidTypeError(std::format("unexpected optional element in value of type struct: element key {}", key));
                }
                values.push_back(Named{
                    uint16_t(key),
                    JSONToValue(value, typeIt->second),
                });
            }
            return values;
        }
        throw UnexpectedTypeError("list");
    }

    Value JSONToValue(const nlohmann::json& json, const ReferenceType& type) {
        if (json.is_string()) {
            const auto& str = json.get<std::string_view>();
            if (!str.starts_with("ref:")) {
                throw InvalidFormatError("reference must be prefixed by 'ref:'");
            }
            const Uid value = stouid(str.substr(4));
            return ToBytes(uint64_t(value));
        }
        throw UnexpectedTypeError("string");
    }

    Value JSONToValue(const nlohmann::json& json, const NameValueUintegerType& type) {
        if (!json.contains("name") || !json.contains("value")) {
            throw UnexpectedTypeError(R"(object: { "name": ..., "value": ... })");
        }
        const auto& nameJson = json["name"];
        const auto& valueJson = json["value"];

        if (!nameJson.is_number_integer()) {
            throw UnexpectedTypeError("integer");
        }

        const auto name = nameJson.get<uint16_t>();
        const auto value = JSONToValue(valueJson, type.ValueType());

        if (name != type.Name()) {
            throw InvalidTypeError(std::format("expected name to be {}, got {}", type.Name(), name));
        }

        return Value(Named(name, value));
    }

} // namespace
} // namespace impl


std::string GetTypeStr(const Type& type) {
    if (type_isa<IntegerType>(type)) {
        return impl::GetTypeStr(type_cast<IntegerType>(type));
    }
    else if (type_isa<BytesType>(type)) {
        return impl::GetTypeStr(type_cast<BytesType>(type));
    }
    else if (type_isa<AlternativeType>(type)) {
        return impl::GetTypeStr(type_cast<AlternativeType>(type));
    }
    else if (type_isa<ListType>(type)) {
        return impl::GetTypeStr(type_cast<ListType>(type));
    }
    else if (type_isa<StructType>(type)) {
        return impl::GetTypeStr(type_cast<StructType>(type));
    }
    else if (type_isa<RestrictedReferenceType>(type)) {
        return impl::GetTypeStr(type_cast<RestrictedReferenceType>(type));
    }
    else if (type_isa<GeneralReferenceType>(type)) {
        return impl::GetTypeStr(type_cast<GeneralReferenceType>(type));
    }
    else if (type_isa<NameValueUintegerType>(type)) {
        return impl::GetTypeStr(type_cast<NameValueUintegerType>(type));
    }
    throw NotImplementedError(std::format("Type to std::string for '{}'", typeid(type).name()));
}


nlohmann::json ValueToJSON(const Value& value, const Type& type) {
    if (type_isa<EnumerationType>(type)) {
        return impl::ValueToJSON(value, type_cast<EnumerationType>(type));
    }
    else if (type_isa<IntegerType>(type)) {
        return impl::ValueToJSON(value, type_cast<IntegerType>(type));
    }
    else if (type_isa<BytesType>(type)) {
        return impl::ValueToJSON(value, type_cast<BytesType>(type));
    }
    else if (type_isa<AlternativeType>(type)) {
        return impl::ValueToJSON(value, type_cast<AlternativeType>(type));
    }
    else if (type_isa<ListType>(type)) {
        return impl::ValueToJSON(value, type_cast<ListType>(type));
    }
    else if (type_isa<StructType>(type)) {
        return impl::ValueToJSON(value, type_cast<StructType>(type));
    }
    else if (type_isa<ReferenceType>(type)) {
        return impl::ValueToJSON(value, type_cast<ReferenceType>(type));
    }
    else if (type_isa<NameValueUintegerType>(type)) {
        return impl::ValueToJSON(value, type_cast<NameValueUintegerType>(type));
    }
    throw NotImplementedError(std::format("Value to JSON for '{}'", typeid(type).name()));
}


Value JSONToValue(const nlohmann::json& value, const Type& type) {
    if (type_isa<EnumerationType>(type)) {
        return impl::JSONToValue(value, type_cast<EnumerationType>(type));
    }
    else if (type_isa<IntegerType>(type)) {
        return impl::JSONToValue(value, type_cast<IntegerType>(type));
    }
    else if (type_isa<BytesType>(type)) {
        return impl::JSONToValue(value, type_cast<BytesType>(type));
    }
    else if (type_isa<AlternativeType>(type)) {
        return impl::JSONToValue(value, type_cast<AlternativeType>(type));
    }
    else if (type_isa<ListType>(type)) {
        return impl::JSONToValue(value, type_cast<ListType>(type));
    }
    else if (type_isa<StructType>(type)) {
        return impl::JSONToValue(value, type_cast<StructType>(type));
    }
    else if (type_isa<ReferenceType>(type)) {
        return impl::JSONToValue(value, type_cast<ReferenceType>(type));
    }
    else if (type_isa<NameValueUintegerType>(type)) {
        return impl::JSONToValue(value, type_cast<NameValueUintegerType>(type));
    }
    throw NotImplementedError(std::format("JSON to Value for '{}'", typeid(type).name()));
}