#pragma once

#include "TableDesc.hpp"

#include <Messaging/NativeTypes.hpp>

#include <algorithm>
#include <cassert>
#include <ranges>
#include <regex>
#include <span>
#include <unordered_map>


namespace sedmgr {

struct ColumnDescStatic {
    std::string_view name;
    bool isUnique;
    const Uid type;
};


struct TableDescStatic {
    std::string_view name;
    eTableKind kind;
    std::span<const ColumnDescStatic> columns = {};
    std::optional<Uid> singleRow = std::nullopt;
};


constexpr Uid TableToDescriptor(Uid table) {
    return (uint64_t(table) >> 32) | (1ull << 32);
}


constexpr Uid DescriptorToTable(Uid descriptor) {
    return uint64_t(descriptor) << 32;
}


class NameSequence {
public:
    constexpr NameSequence(Uid base, uint64_t start, uint64_t count, std::format_string<uint64_t> format)
        : base(base), start(start), count(count), format(format) {
        assert(format.get().find("{}") != format.get().npos);
    }

    std::optional<std::string> Find(Uid uid) const;
    std::optional<Uid> Find(std::string_view name) const;

private:
    Uid base;
    uint64_t start;
    uint64_t count;
    std::format_string<uint64_t> format;
};


class NameAndUidFinder {
public:
    template <class PairRanges = std::initializer_list<std::initializer_list<std::pair<Uid, std::string_view>>>,
              class SequenceRanges = std::initializer_list<NameSequence>>
    NameAndUidFinder(PairRanges&& pairs, SequenceRanges&& sequences);

    std::optional<std::string> Find(Uid uid) const;
    std::optional<Uid> Find(std::string_view name) const;

private:
    std::unordered_map<Uid, std::string_view> m_uidToName;
    std::unordered_map<std::string_view, Uid> m_nameToUid;
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
                throw std::invalid_argument(std::format("all UIDs must be unique, duplicate: {}", to_string(pair.first)));
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
    SPNameAndUidFinder(std::unordered_map<Uid, NameAndUidFinder> finders) : m_finders(std::move(finders)) {}

    std::optional<std::string> Find(Uid uid, Uid sp) const;
    std::optional<Uid> Find(std::string_view name, Uid sp) const;

private:
    std::unordered_map<Uid, NameAndUidFinder> m_finders;
};

} // namespace sedmgr