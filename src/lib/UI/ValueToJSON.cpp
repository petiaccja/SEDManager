#include "ValueToJSON.hpp"

#include "Exception.hpp"

#include <Specification/ColumnTypes.hpp>
#include <Specification/Names.hpp>

#include <sstream>

// Types:
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
            ss << GetTypeStr(alt);
            if (&alt != &altTypes.back()) {
                ss << " | ";
            }
        }
        ss << " }";
        return ss.str();
    }

    std::string GetTypeStr(const ListType& type) {
        return std::format("list{{ {} }}>", GetTypeStr(type.ElementType()));
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
            ss << GetNameOrUid(table);
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

    //--------------------------------------------------------------------------
    // Get JSON from value
    //--------------------------------------------------------------------------

    bool InterpretAsString(const BytesType& type) {
        try {
            const auto uid = type_uid(type);
            if (uid == type_uid(column_types::name) || uid == type_uid(column_types::password)) {
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

    nlohmann::json ValueToJSON(const Value& value, const IntegerType& type) {
        if (value.IsInteger()) {
            if (type.Signedness()) {
                return nlohmann::json(value.Get<int64_t>());
            }
            else {
                return nlohmann::json(value.Get<uint64_t>());
            }
        }
        throw TypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const BytesType& type) {
        if (value.IsBytes()) {
            return BytesToJSON(value.AsBytes(), "", InterpretAsString(type));
        }
        throw TypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const AlternativeType& type) {
        if (value.IsNamed()) {
            const auto& alts = type.Types();
            const auto currentTypeType = BytesType(4, true);
            const Value& currentType = value.AsNamed().name;
            const Value& currentValue = value.AsNamed().value;
            if (!currentType.IsBytes()) {
                throw TypeError(GetTypeStr(currentTypeType), value.GetTypeStr());
            }
            const auto& typeIdBytes = currentType.AsBytes();
            uint64_t altId = 0;
            for (const auto& byte : typeIdBytes) {
                altId <<= 8;
                altId |= uint8_t(byte);
            }
            const auto altIter = std::ranges::find_if(alts, [&altId](const Type& alt) {
                try {
                    return static_cast<uint32_t>(type_uid(alt)) == altId;
                }
                catch (std::bad_cast&) {
                    throw std::invalid_argument(std::format("expected an identified type, got {}", GetTypeStr(alt)));
                }
            });
            if (altIter == alts.end()) {
                throw TypeError(GetTypeStr(type), std::format("UID{:08x}", altId));
            }

            return nlohmann::json({
                {"type",   ValueToJSON(currentType,  currentTypeType)},
                { "value", ValueToJSON(currentValue, *altIter)       },
            });
        }
        throw TypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const ListType& type) {
        if (value.IsList()) {
            std::vector<nlohmann::json> jsons;
            const auto& items = value.AsList();
            for (auto& item : items) {
                jsons.push_back(ValueToJSON(item, type.ElementType()));
            }
            return nlohmann::json(jsons);
        }
        throw TypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const StructType& type) {
        if (value.IsList()) {
            const auto& elements = value.AsList();
            const auto& elementTypes = type.ElementTypes();

            auto mandatoryElements = elements | std::views::filter([](const Value& v) { return !v.IsNamed(); });
            auto mandatoryTypes = elementTypes | std::views::filter([](const Type& t) { return !type_isa<NameValueUintegerType>(t); });
            std::vector<std::pair<const Value&, const Type&>> mandatories;
            auto [meit, mtit] = std::tuple(begin(mandatoryElements), begin(mandatoryTypes));
            for (; meit != end(mandatoryElements) && mtit != end(mandatoryTypes); ++meit, ++mtit) {
                mandatories.emplace_back(*meit, *mtit);
            }
            if (meit != end(mandatoryElements) || mtit != end(mandatoryTypes)) {
                throw std::logic_error(std::format("value does not contain the same number of mandatory fields specified in {}", GetTypeStr(type)));
            }

            auto optionalElements = elements | std::views::filter([](const Value& v) { return v.IsNamed(); });
            auto optionalTypes = elementTypes | std::views::filter([](const Type& t) { return type_isa<NameValueUintegerType>(t); });

            std::unordered_map<uint64_t, Type> optionalTypeLookup;
            for (const auto& optionalType : optionalTypes) {
                const auto& namedType = type_cast<NameValueUintegerType>(optionalType);
                optionalTypeLookup.insert({ namedType.NameType(), namedType.ValueType() });
            }

            std::vector<nlohmann::json> jsons;
            for (const auto& mandatoryElement : mandatories) {
                jsons.emplace_back(ValueToJSON(mandatoryElement.first, mandatoryElement.second));
            }

            for (const auto& optionalElement : optionalElements) {
                const auto& named = optionalElement.AsNamed();
                if (!named.name.IsInteger()) {
                    throw std::logic_error("optional element of struct type must have integer key");
                }
                const auto key = named.name.Get<uint64_t>();
                const auto typeIt = optionalTypeLookup.find(key);
                if (typeIt == optionalTypeLookup.end()) {
                    throw std::logic_error("unexpected optional element in value of type struct");
                }
                jsons.push_back({
                    { "field", key },
                    { "value", ValueToJSON(named.value, typeIt->second) },
                });
            }
            return nlohmann::json(jsons);
        }
        throw TypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const RestrictedReferenceType& type) {
        if (value.IsBytes()) {
            return BytesToJSON(value.AsBytes(), "ref:", false);
        }
        throw TypeError(GetTypeStr(type), value.GetTypeStr());
    }

    nlohmann::json ValueToJSON(const Value& value, const GeneralReferenceType& type) {
        if (value.IsBytes()) {
            return BytesToJSON(value.AsBytes(), "ref:", false);
        }
        throw TypeError(GetTypeStr(type), value.GetTypeStr());
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
    throw std::logic_error("cannot convert type to string: not implemented");
}


nlohmann::json ValueToJSON(const Value& value, const Type& type) {
    if (type_isa<IntegerType>(type)) {
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
    else if (type_isa<RestrictedReferenceType>(type)) {
        return impl::ValueToJSON(value, type_cast<RestrictedReferenceType>(type));
    }
    else if (type_isa<GeneralReferenceType>(type)) {
        return impl::ValueToJSON(value, type_cast<GeneralReferenceType>(type));
    }
    throw std::logic_error("not implemented");
}


Value JSONToValue(const nlohmann::json& value, const Type& type) {
    throw std::logic_error("not implemented");
}