#pragma once

#include "TableDesc.hpp"

#include <Messaging/Native.hpp>

#include <algorithm>
#include <cassert>
#include <concepts>
#include <regex>
#include <span>
#include <unordered_map>


namespace sedmgr {

struct ColumnDescStatic {
    std::string_view name;
    bool isUnique;
    const UID type;
};


struct TableDescStatic {
    std::string_view name;
    eTableKind kind;
    std::span<const ColumnDescStatic> columns = {};
    std::optional<UID> singleRow = std::nullopt;
};


class NameSequence {
public:
    constexpr NameSequence(UID base, uint64_t start, uint64_t count, std::format_string<uint64_t> format)
        : base(base), start(start), count(count), format(format) {
        assert(format.get().find("{}") != format.get().npos);
    }

    std::optional<std::string> Find(UID uid) const;
    std::optional<UID> Find(std::string_view name) const;

private:
    UID base;
    uint64_t start;
    uint64_t count;
    std::format_string<uint64_t> format;
};


class NameAndUidFinder {
public:
    template <class PairRanges = std::initializer_list<std::initializer_list<std::pair<UID, std::string_view>>>,
              class SequenceRanges = std::initializer_list<NameSequence>>
    NameAndUidFinder(PairRanges&& pairs, SequenceRanges&& sequences);

    std::optional<std::string> Find(UID uid) const;
    std::optional<UID> Find(std::string_view name) const;

private:
    std::unordered_map<UID, std::string_view> m_uidToName;
    std::unordered_map<std::string_view, UID> m_nameToUid;
    std::vector<NameSequence> m_sequences;
};


template <class PairRanges,
          class SequenceRange>
NameAndUidFinder::NameAndUidFinder(PairRanges&& pairRanges, SequenceRange&& sequences) {
    for (auto& pairRange : pairRanges) {
        for (auto& pair : pairRange) {
            const auto [it1, ins1] = m_uidToName.insert({ pair.first, pair.second });
            const auto [it2, ins2] = m_nameToUid.insert({ pair.second, pair.first });
            if (!ins1) {
                throw std::invalid_argument(std::format("all UIDs must be unique, duplicate: {}", pair.first.ToString()));
            }
            if (!ins2) {
                throw std::invalid_argument(std::format("all names must be unique, duplicate: {}", pair.second));
            }
        }
    }
    for (auto&& seq : sequences) {
        m_sequences.push_back(seq);
    }
}


class SPNameAndUidFinder {
public:
    SPNameAndUidFinder(std::unordered_map<UID, NameAndUidFinder> finders) : m_finders(std::move(finders)) {}

    std::optional<std::string> Find(UID uid, UID sp) const;
    std::optional<UID> Find(std::string_view name, UID sp) const;

private:
    std::unordered_map<UID, NameAndUidFinder> m_finders;
};


class TableFinder {
public:
    template <std::ranges::range PairRange>
    explicit TableFinder(PairRange&& descs) {
        m_lut = [&descs] {
            std::unordered_map<UID, TableDescStatic> lut;
            for (auto& [uid, desc] : descs) {
                lut.insert({ uid, desc });
            }
            return lut;
        }();
    }

    std::optional<TableDesc> Find(UID table, std::function<std::optional<Type>(UID)> findType) const {
        const auto it = m_lut.find(table);
        if (it != m_lut.end()) {
            std::vector<ColumnDesc> columns;
            for (const auto& column : it->second.columns) {
                columns.emplace_back(std::string(column.name), column.isUnique, findType(column.type).value());
            }
            return TableDesc{ std::string(it->second.name), it->second.kind, std::move(columns), it->second.singleRow };
        }
        return std::nullopt;
    }

private:
    std::unordered_map<UID, TableDescStatic> m_lut;
};

} // namespace sedmgr