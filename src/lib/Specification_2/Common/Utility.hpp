#pragma once

#include "TableDesc.hpp"

#include <Data/NativeTypes.hpp>

#include <algorithm>
#include <cassert>
#include <ranges>
#include <regex>
#include <span>
#include <unordered_map>


struct ColumnDescStatic {
    std::string_view name;
    bool isUnique;
    const Type& type;
    operator ColumnDesc() const {
        return { std::string(name), isUnique, type };
    }
};


struct TableDescStatic {
    Uid uid;
    std::string_view name;
    eTableKind kind;
    std::span<const ColumnDescStatic> columns = {};
    std::optional<Uid> singleRow = std::nullopt;
    operator TableDesc() const {
        std::vector<ColumnDesc> columns_;
        std::ranges::transform(columns, std::back_inserter(columns_), [](const ColumnDescStatic& v) { return ColumnDesc(v); });
        return { std::string(name), kind, std::move(columns_), singleRow };
    }
};


template <class Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, std::pair<Uid, std::string_view>>
std::unordered_map<Uid, std::string_view> MakeNameLookup(std::initializer_list<Range> pairings) {
    std::unordered_map<Uid, std::string_view> lut;
    for (const auto& pairing : pairings) {
        for (const auto& item : pairing) {
            const auto [it, newlyInserted] = lut.insert({ item.first, item.second });
            assert(newlyInserted);
        }
    }
    return lut;
}


template <class Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, std::pair<Uid, std::string_view>>
std::unordered_map<std::string_view, Uid> MakeUidLookup(std::initializer_list<Range> pairings) {
    std::unordered_map<std::string_view, Uid> lut;
    for (const auto& pairing : pairings) {
        for (const auto& item : pairing) {
            const auto [it, newlyInserted] = lut.insert({ item.second, item.first });
            assert(newlyInserted);
        }
    }
    return lut;
}


constexpr Uid TableToDescriptor(Uid table) {
    return (uint64_t(table) >> 32) | (1ull << 32);
}


constexpr Uid DescriptorToTable(Uid descriptor) {
    return uint64_t(descriptor) << 32;
}


struct NameSequence {
    NameSequence(Uid base, uint64_t start, uint64_t count, std::format_string<uint64_t> format)
        : base(base), start(start), count(count), format(format), parse(std::regex_replace(format.get().data(), std::regex("{}"), "([0-9]*)")) {
        assert(format.get().find("{}") != format.get().npos);
    }
    Uid base;
    uint64_t start;
    uint64_t count;
    std::format_string<uint64_t> format;
    std::regex parse;
};


inline std::optional<std::string> FindNameSequence(Uid uid, const NameSequence& desc) {
    const auto index = int64_t(uid) - int64_t(desc.base);
    if (0 < index && index < int64_t(desc.count)) {
        const auto number = index + desc.start;
        return std::format(desc.format, uint64_t(number));
    }
    return std::nullopt;
}


inline std::optional<Uid> FindUidSequence(std::string_view name, const NameSequence& desc) {
    std::match_results<std::string_view::iterator> matches;
    const bool success = std::regex_match(name.begin(), name.end(), matches, desc.parse);
    if (success) {
        assert(matches.size() == 2);
        try {
            const auto number = std::stoll(matches[1].str());
            const auto index = number - int64_t(desc.start);
            if (0 < index && index < int64_t(desc.count)) {
                return Uid(uint64_t(desc.base) + index);
            }
        }
        catch (std::exception&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}