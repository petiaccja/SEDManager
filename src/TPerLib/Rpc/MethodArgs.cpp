#include "MethodArgs.hpp"

#include "../Serialization/Utility.hpp"


Value SerializeArg(const std::string& arg) {
    return SerializeArg(std::string_view(arg));
}


Value SerializeArg(std::string_view arg) {
    return { Value::bytes, arg };
}


Value SerializeArg(const Uid& arg) {
    return { Value::bytes, ToBytes(arg.value) };
}


void ParseArg(const Value& stream, Uid& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    const auto bytes = stream.Get<std::span<const uint8_t>>();
    if (bytes.size() != 8) {
        throw std::invalid_argument("a UID is 8 bytes");
    }
    uint64_t value;
    FromBytes(bytes, value);
    arg = value;
}


void ParseArg(const Value& stream, std::string& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    arg = std::string(stream.Get<std::string_view>());
}


void ParseArg(const Value& stream, std::vector<std::byte>& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    auto bytes = stream.Get<std::span<const std::byte>>();
    arg = { bytes.begin(), bytes.end() };
}


Value SerializeArg(const Value& arg) {
    return arg;
}

void ParseArg(const Value& value, Value& arg) {
    arg = value;
}


Value SerializeArg(const CellBlock& arg) {
    std::vector<Value> fields;
    if (arg.startRow.HasValue()) {
        fields.emplace_back(Named{ 1u, arg.startRow });
    }
    if (arg.endRow.HasValue()) {
        fields.emplace_back(Named{ 2u, arg.endRow });
    }
    if (arg.startColumn) {
        fields.emplace_back(Named{ 3u, arg.startColumn.value() });
    }
    if (arg.endColumn) {
        fields.emplace_back(Named{ 4u, arg.endColumn.value() });
    }
    return fields;
}

void ParseArg(const Value& value, CellBlock& arg) {
    if (!value.IsList()) {
        throw std::invalid_argument("expected a list");
    }
    const auto fields = value.Get<std::span<const Value>>();
    CellBlock parsed;
    for (const auto& field : fields) {
        const auto& named = field.Get<Named>();
        const auto id = named.name.Get<uint32_t>();
        switch (id) {
            case 0: break; // Table name
            case 1: parsed.startRow = named.value; break;
            case 2: parsed.endRow = named.value; break;
            case 3: parsed.startColumn = named.value.Get<uint32_t>(); break;
            case 4: parsed.endColumn = named.value.Get<uint32_t>(); break;
            default: throw std::invalid_argument("invalid field for cell block");
        }
    }
    arg = std::move(parsed);
}