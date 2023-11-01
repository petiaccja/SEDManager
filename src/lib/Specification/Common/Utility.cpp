#include "Utility.hpp"


namespace sedmgr {

std::optional<std::string> NameSequence::Find(Uid uid) const {
    const auto index = int64_t(uid) - int64_t(base);
    if (0 <= index && index < int64_t(count)) {
        const auto number = index + start;
        return std::format(format, uint64_t(number));
    }
    return std::nullopt;
}


std::optional<Uid> NameSequence::Find(std::string_view name) const {
    std::match_results<std::string_view::iterator> matches;
    const bool success = std::regex_match(name.begin(), name.end(), matches, parse);
    if (success) {
        assert(matches.size() == 2);
        try {
            const auto number = std::stoll(matches[1].str());
            const auto index = number - int64_t(start);
            if (0 <= index && index < int64_t(count)) {
                return Uid(uint64_t(base) + index);
            }
        }
        catch (std::exception&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}


std::optional<std::string> NameAndUidFinder::Find(Uid uid) const {
    const auto it = m_uidToName.find(uid);
    if (it != m_uidToName.end()) {
        return std::string(it->second);
    }

    for (const auto& sequence : m_sequences) {
        auto result = sequence.Find(uid);
        if (result) {
            return result;
        }
    }
    return std::nullopt;
}


std::optional<Uid> NameAndUidFinder::Find(std::string_view name) const {
    const auto it = m_nameToUid.find(name);
    if (it != m_nameToUid.end()) {
        return it->second;
    }

    for (const auto& sequence : m_sequences) {
        auto result = sequence.Find(name);
        if (result) {
            return result;
        }
    }
    return std::nullopt;
}


std::optional<std::string> SPNameAndUidFinder::Find(Uid uid, Uid sp) const {
    const auto it = m_finders.find(sp);
    if (it != m_finders.end()) {
        return it->second.Find(uid);
    }
    return std::nullopt;
}


std::optional<Uid> SPNameAndUidFinder::Find(std::string_view name, Uid sp) const {
    const auto it = m_finders.find(sp);
    if (it != m_finders.end()) {
        return it->second.Find(name);
    }
    return std::nullopt;
}

} // namespace sedmgr