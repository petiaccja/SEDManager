#include "ValueToJSON.hpp"

#include <Archive/Serialization.hpp>
#include <Error/Exception.hpp>
#include <Specification/Core/CoreModule.hpp>

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
// ReferenceType
// - RestrictedReferenceType
//   - RestrictedByteReferenceType
//   - RestrictedObjectReferenceType
// - GeneralReferenceType
//   - GeneralByteReferenceType
//   - GeneralObjectReferenceType
//     - GeneralTableReferenceType
//     - GeneralByteTableReferenceType
//     - GeneralObjectTableReferenceType

namespace sedmgr {

namespace impl {
    namespace {

        struct ReducedStruct {
            std::vector<Type> mandatoryFields;
            std::unordered_map<uint64_t, Type> optionalFields;
        };


        bool InterpretAsString(const BytesType& type);
        nlohmann::json BytesToJSON(std::span<const std::byte> bytes, std::string_view prefix, bool asString);
        std::vector<std::byte> JSONToBytes(nlohmann::json json, std::string_view prefix, bool asString);
        bool IsOptionalField(const nlohmann::json& json);
        ReducedStruct ReduceStructType(const StructType& type);


        class TypeFormatter {
        public:
            std::string Format(const Type& type) const;
            std::string operator()(const Type& type) const { return Format(type); }

        private:
            std::string Format(const IntegerType& type) const;
            std::string Format(const BytesType& type) const;
            std::string Format(const AlternativeType& type) const;
            std::string Format(const ListType& type) const;
            std::string Format(const StructType& type) const;
            std::string Format(const RestrictedReferenceType& type) const;
            std::string Format(const GeneralReferenceType& type) const;
            std::string Format(const NameValueUintegerType& type) const;
        };


        class ValueToJSONConverter {
        public:
            ValueToJSONConverter(std::function<std::optional<std::string>(UID)> nameConverter = {})
                : m_nameConverter(nameConverter) {}

            nlohmann::json Convert(const Value& value, const Type& type) const;
            nlohmann::json operator()(const Value& value, const Type& type) const { return Convert(value, type); }

        private:
            nlohmann::json Convert(const Value& value, const EnumerationType& type) const;
            nlohmann::json Convert(const Value& value, const IntegerType& type) const;
            nlohmann::json Convert(const Value& value, const BytesType& type) const;
            nlohmann::json Convert(const Value& value, const AlternativeType& type) const;
            nlohmann::json Convert(const Value& value, const ListType& type) const;
            nlohmann::json Convert(const Value& value, const StructType& type) const;
            nlohmann::json Convert(const Value& value, const ReferenceType& type) const;
            nlohmann::json Convert(const Value& value, const NameValueUintegerType& type) const;

        private:
            TypeFormatter m_formatter;
            std::function<std::optional<std::string>(UID)> m_nameConverter;
        };


        class JSONToValueConverter {
        public:
            JSONToValueConverter(std::function<std::optional<UID>(std::string_view)> nameConverter = {})
                : m_nameConverter(nameConverter) {}
            Value Convert(const nlohmann::json& value, const Type& type) const;
            Value operator()(const nlohmann::json& value, const Type& type) const { return Convert(value, type); }

        private:
            Value Convert(const nlohmann::json& value, const EnumerationType& type) const;
            Value Convert(const nlohmann::json& value, const IntegerType& type) const;
            Value Convert(const nlohmann::json& value, const BytesType& type) const;
            Value Convert(const nlohmann::json& value, const AlternativeType& type) const;
            Value Convert(const nlohmann::json& value, const ListType& type) const;
            Value Convert(const nlohmann::json& value, const StructType& type) const;
            Value Convert(const nlohmann::json& value, const ReferenceType& type) const;
            Value Convert(const nlohmann::json& value, const NameValueUintegerType& type) const;

        private:
            TypeFormatter m_formatter;
            std::function<std::optional<UID>(std::string_view)> m_nameConverter;
        };


        std::string TypeFormatter::Format(const IntegerType& type) const {
            return std::format("{}integer_{}", type.Signedness() ? "" : "u", type.Width());
        }

        std::string TypeFormatter::Format(const BytesType& type) const {
            return std::format("{}bytes_{}", type.Fixed() ? "" : "max_", type.Length());
        }

        std::string TypeFormatter::Format(const AlternativeType& type) const {
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
                ss << TypeFormatter::Format(alt);
                if (&alt != &altTypes.back()) {
                    ss << " | ";
                }
            }
            ss << " }";
            return ss.str();
        }

        std::string TypeFormatter::Format(const ListType& type) const {
            return std::format("list{{ {} }}", Format(type.ElementType()));
        }

        std::string TypeFormatter::Format(const StructType& type) const {
            std::stringstream ss;
            ss << "struct{ ";
            const auto& elementTypes = type.ElementTypes();
            for (const auto& alt : elementTypes) {
                ss << Format(alt);
                if (&alt != &elementTypes.back()) {
                    ss << ", ";
                }
            }
            ss << " }";
            return ss.str();
        }

        std::string TypeFormatter::Format(const RestrictedReferenceType& type) const {
            std::stringstream ss;
            ss << "{ ";
            const auto& tables = type.Tables();
            for (const auto& table : tables) {
                ss << CoreModule::Get()->FindName(table).value_or(UID(table).ToString());
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

        std::string TypeFormatter::Format(const GeneralReferenceType& type) const {
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

        std::string TypeFormatter::Format(const NameValueUintegerType& type) const {
            return std::format("named{{ {}: {} }}", type.Name(), Format(type.ValueType()));
        }

        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const EnumerationType& type) const {
            if (value.IsInteger()) {
                const auto enumValue = value.Get<uint16_t>();
                const auto& lookupByValue = type.ByValue();
                const auto it = lookupByValue.find(enumValue);
                if (it != lookupByValue.end()) {
                    return nlohmann::json(it->second);
                }
                return nlohmann::json(enumValue);
            }
            throw UnexpectedTypeError(m_formatter(type), value.GetTypeStr());
        }

        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const IntegerType& type) const {
            if (value.IsInteger()) {
                if (type.Signedness()) {
                    return nlohmann::json(value.Get<int64_t>());
                }
                else {
                    return nlohmann::json(value.Get<uint64_t>());
                }
            }
            throw UnexpectedTypeError(m_formatter(type), value.GetTypeStr());
        }

        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const BytesType& type) const {
            if (value.Is<Bytes>()) {
                return BytesToJSON(value.Get<Bytes>(), "", InterpretAsString(type));
            }
            throw UnexpectedTypeError(m_formatter(type), value.GetTypeStr());
        }

        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const AlternativeType& type) const {
            if (value.Is<Named>()) {
                const auto& alts = type.Types();
                const auto currentTypeType = BytesType(4, true);
                const Value& currentType = value.Get<Named>().name;
                const Value& currentValue = value.Get<Named>().value;
                if (!currentType.Is<Bytes>()) {
                    throw UnexpectedTypeError(m_formatter(currentTypeType), value.GetTypeStr());
                }
                const auto altUidLower = DeSerialize(Serialized<uint32_t>{ currentType.Get<Bytes>() });
                const UID altUid = UID(uint64_t(altUidLower) | 0x0000'0005'0000'0000);
                const auto altIter = std::ranges::find_if(alts, [this, &altUid](const Type& alt) {
                    try {
                        return type_uid(alt) == altUid;
                    }
                    catch (std::bad_cast&) {
                        throw UnexpectedTypeError("<any identified type>", m_formatter(alt));
                    }
                });
                if (altIter == alts.end()) {
                    throw UnexpectedTypeError(m_formatter(type), "uid:" + altUid.ToString());
                }

                return nlohmann::json({
                    {Convert(Serialize(uint64_t(altUid)), ReferenceType{}), Convert(currentValue, *altIter)}
                });
            }
            throw UnexpectedTypeError(m_formatter(type), value.GetTypeStr());
        }

        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const ListType& type) const {
            if (value.Is<List>()) {
                std::vector<nlohmann::json> jsons;
                const auto& items = value.Get<List>();
                for (auto& item : items) {
                    jsons.push_back(Convert(item, type.ElementType()));
                }
                return nlohmann::json(jsons);
            }
            throw UnexpectedTypeError(m_formatter(type), value.GetTypeStr());
        }

        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const StructType& type) const {
            if (value.Is<List>()) {
                const auto& elements = value.Get<List>();

                const auto [mandatoryTypes, optionalTypeLookup] = ReduceStructType(type);
                auto mandatoryElements = elements | std::views::filter([](const Value& v) { return !v.Is<Named>(); });
                auto optionalElements = elements | std::views::filter([](const Value& v) { return v.Is<Named>(); });

                std::vector<nlohmann::json> jsons;

                auto [meit, mtit] = std::tuple(begin(mandatoryElements), begin(mandatoryTypes));
                for (; meit != end(mandatoryElements) && mtit != end(mandatoryTypes); ++meit, ++mtit) {
                    jsons.emplace_back(Convert(*meit, *mtit));
                }
                if (meit != end(mandatoryElements) || mtit != end(mandatoryTypes)) {
                    throw InvalidTypeError(std::format("value does not contain the same number of mandatory fields specified in type '{}'", m_formatter(type)));
                }

                for (const auto& optionalElement : optionalElements) {
                    const auto& named = optionalElement.Get<Named>();
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
                        { "value", Convert(named.value, typeIt->second) },
                    });
                }
                return nlohmann::json(jsons);
            }
            throw UnexpectedTypeError(m_formatter(type), value.GetTypeStr());
        }

        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const ReferenceType& type) const {
            if (value.Is<Bytes>()) {
                const auto uid = DeSerialize(Serialized<UID>{ value.Get<Bytes>() });
                if (m_nameConverter) {
                    const auto maybeName = m_nameConverter(uid);
                    if (maybeName) {
                        return "ref:" + *maybeName;
                    }
                }
                return "ref:" + uid.ToString();
            }
            throw UnexpectedTypeError(m_formatter(type), value.GetTypeStr());
        }

        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const NameValueUintegerType& type) const {
            if (value.Is<Named>()) {
                const auto& name = value.Get<Named>().name;
                if (!name.IsInteger()) {
                    throw UnexpectedTypeError("integer", name.GetTypeStr());
                }
                if (name.Get<uint16_t>() != type.Name()) {
                    throw InvalidTypeError(std::format("name encoded in Value ({}) does not match name encoded in Type ({})", name.Get<uint16_t>(), type.Name()));
                }
                return nlohmann::json({
                    { "name", type.Name() },
                    { "value", Convert(value.Get<Named>().value, type.ValueType()) },
                });
            }
            throw UnexpectedTypeError(m_formatter(type), value.GetTypeStr());
        }


        Value JSONToValueConverter::Convert(const nlohmann::json& json, const EnumerationType& type) const {
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


        Value JSONToValueConverter::Convert(const nlohmann::json& json, const IntegerType& type) const {
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

        Value JSONToValueConverter::Convert(const nlohmann::json& json, const BytesType& type) const {
            return JSONToBytes(json, "", InterpretAsString(type));
        }

        Value JSONToValueConverter::Convert(const nlohmann::json& json, const AlternativeType& type) const {
            if (!json.is_object() || json.empty()) {
                throw UnexpectedTypeError("object: { type: value }", json.type_name());
            }
            const auto& altJson = json.begin().key();
            const auto& valueJson = json.begin().value();
            const UID altUid = DeSerialize(Serialized<UID>{ Convert(altJson, ReferenceType{}).Get<Bytes>() });

            const auto& alts = type.Types();
            const auto altIter = std::ranges::find_if(alts, [this, &altUid](const Type& alt) {
                try {
                    return type_uid(alt) == altUid;
                }
                catch (std::bad_cast&) {
                    throw UnexpectedTypeError("<any identified type>", m_formatter(alt));
                }
            });
            if (altIter == alts.end()) {
                throw UnexpectedTypeError(m_formatter(type), "uid:" + altUid.ToString());
            }

            return Named{
                Serialize(uint32_t(uint64_t(altUid))),
                Convert(valueJson, *altIter),
            };
        }

        Value JSONToValueConverter::Convert(const nlohmann::json& json, const ListType& type) const {
            if (!json.is_array()) {
                throw UnexpectedTypeError("list");
            }
            std::vector<Value> values;
            for (const auto& item : json) {
                values.push_back(Convert(item, type.ElementType()));
            }
            return values;
        }

        Value JSONToValueConverter::Convert(const nlohmann::json& json, const StructType& type) const {
            if (json.is_array()) {
                const auto& elements = json;

                const auto [mandatoryTypes, optionalTypeLookup] = ReduceStructType(type);
                auto mandatoryElements = elements | std::views::filter([](const auto& v) { return !IsOptionalField(v); });
                auto optionalElements = elements | std::views::filter([](const auto& v) { return IsOptionalField(v); });

                std::vector<Value> values;

                auto [meit, mtit] = std::tuple(begin(mandatoryElements), begin(mandatoryTypes));
                for (; meit != end(mandatoryElements) && mtit != end(mandatoryTypes); ++meit, ++mtit) {
                    values.emplace_back(Convert(*meit, *mtit));
                }
                if (meit != end(mandatoryElements) || mtit != end(mandatoryTypes)) {
                    throw InvalidTypeError(std::format("value does not contain the same number of mandatory fields specified in '{}'", m_formatter(type)));
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
                        Convert(value, typeIt->second),
                    });
                }
                return values;
            }
            throw UnexpectedTypeError("list");
        }

        Value JSONToValueConverter::Convert(const nlohmann::json& json, const ReferenceType& type) const {
            if (json.is_string()) {
                const auto& str = json.get<std::string_view>();
                if (!str.starts_with("ref:")) {
                    throw InvalidFormatError("reference must be prefixed by 'ref:'");
                }
                if (m_nameConverter) {
                    const auto maybeValue = m_nameConverter(str.substr(4));
                    if (maybeValue) {
                        return Serialize(*maybeValue);
                    }
                }
                const UID value = UID::Parse(str.substr(4));
                return Serialize(value);
            }
            throw UnexpectedTypeError("string");
        }

        Value JSONToValueConverter::Convert(const nlohmann::json& json, const NameValueUintegerType& type) const {
            if (!json.contains("name") || !json.contains("value")) {
                throw UnexpectedTypeError(R"(object: { "name": ..., "value": ... })");
            }
            const auto& nameJson = json["name"];
            const auto& valueJson = json["value"];

            if (!nameJson.is_number_integer()) {
                throw UnexpectedTypeError("integer");
            }

            const auto name = nameJson.get<uint16_t>();
            const auto value = Convert(valueJson, type.ValueType());

            if (name != type.Name()) {
                throw InvalidTypeError(std::format("expected name to be {}, got {}", type.Name(), name));
            }

            return Value(Named(name, value));
        }


        std::string TypeFormatter::Format(const Type& type) const {
            if (type_isa<IntegerType>(type)) {
                return Format(type_cast<IntegerType>(type));
            }
            else if (type_isa<BytesType>(type)) {
                return Format(type_cast<BytesType>(type));
            }
            else if (type_isa<AlternativeType>(type)) {
                Format(type_cast<AlternativeType>(type));
            }
            else if (type_isa<ListType>(type)) {
                return Format(type_cast<ListType>(type));
            }
            else if (type_isa<StructType>(type)) {
                return Format(type_cast<StructType>(type));
            }
            else if (type_isa<RestrictedReferenceType>(type)) {
                return Format(type_cast<RestrictedReferenceType>(type));
            }
            else if (type_isa<GeneralReferenceType>(type)) {
                return Format(type_cast<GeneralReferenceType>(type));
            }
            else if (type_isa<NameValueUintegerType>(type)) {
                return Format(type_cast<NameValueUintegerType>(type));
            }
            throw NotImplementedError(std::format("Type to std::string for '{}'", typeid(type).name()));
        }


        nlohmann::json ValueToJSONConverter::Convert(const Value& value, const Type& type) const {
            if (type_isa<EnumerationType>(type)) {
                return Convert(value, type_cast<EnumerationType>(type));
            }
            else if (type_isa<IntegerType>(type)) {
                return Convert(value, type_cast<IntegerType>(type));
            }
            else if (type_isa<BytesType>(type)) {
                return Convert(value, type_cast<BytesType>(type));
            }
            else if (type_isa<AlternativeType>(type)) {
                return Convert(value, type_cast<AlternativeType>(type));
            }
            else if (type_isa<ListType>(type)) {
                return Convert(value, type_cast<ListType>(type));
            }
            else if (type_isa<StructType>(type)) {
                return Convert(value, type_cast<StructType>(type));
            }
            else if (type_isa<ReferenceType>(type)) {
                return Convert(value, type_cast<ReferenceType>(type));
            }
            else if (type_isa<NameValueUintegerType>(type)) {
                return Convert(value, type_cast<NameValueUintegerType>(type));
            }
            throw NotImplementedError(std::format("Value to JSON for '{}'", typeid(type).name()));
        }


        Value JSONToValueConverter::Convert(const nlohmann::json& value, const Type& type) const {
            if (type_isa<EnumerationType>(type)) {
                return Convert(value, type_cast<EnumerationType>(type));
            }
            else if (type_isa<IntegerType>(type)) {
                return Convert(value, type_cast<IntegerType>(type));
            }
            else if (type_isa<BytesType>(type)) {
                return Convert(value, type_cast<BytesType>(type));
            }
            else if (type_isa<AlternativeType>(type)) {
                return Convert(value, type_cast<AlternativeType>(type));
            }
            else if (type_isa<ListType>(type)) {
                return Convert(value, type_cast<ListType>(type));
            }
            else if (type_isa<StructType>(type)) {
                return Convert(value, type_cast<StructType>(type));
            }
            else if (type_isa<ReferenceType>(type)) {
                return Convert(value, type_cast<ReferenceType>(type));
            }
            else if (type_isa<NameValueUintegerType>(type)) {
                return Convert(value, type_cast<NameValueUintegerType>(type));
            }
            throw NotImplementedError(std::format("JSON to Value for '{}'", typeid(type).name()));
        }

        bool InterpretAsString(const BytesType& type) {
            try {
                const auto uid = type_uid(type);
                if (uid == UID(core::eType::name) || uid == UID(core::eType::password)) {
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

        bool IsOptionalField(const nlohmann::json& json) {
            return json.contains("field") && json.contains("value");
        }

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

    } // namespace
} // namespace impl


std::string GetTypeStr(const Type& type) {
    impl::TypeFormatter formatter;
    return formatter.Format(type);
}


nlohmann::json ValueToJSON(const Value& value, const Type& type, std::function<std::optional<std::string>(UID)> nameConverter) {
    impl::ValueToJSONConverter converter(std::move(nameConverter));
    return converter.Convert(value, type);
}


Value JSONToValue(const nlohmann::json& value, const Type& type, std::function<std::optional<UID>(std::string_view)> nameConverter) {
    impl::JSONToValueConverter converter(std::move(nameConverter));
    return converter.Convert(value, type);
}

} // namespace sedmgr