#include "Names.hpp"

#include "Authorities.hpp"
#include "Methods.hpp"
#include "SecurityProviders.hpp"
#include "Tables.hpp"

#include <array>
#include <unordered_map>
#include <utility>



constexpr std::initializer_list<std::pair<Uid, std::string_view>> securityProviders = {
    { opal::eSecurityProvider::Admin, "Admin" },
    { opal::eSecurityProvider::Locking, "Locking" },
};


constexpr std::initializer_list<std::pair<Uid, std::string_view>> authorities = {
    { eAuthority::Anybody, "Anybody" },
    { eAuthority::Admins, "Admins" },
    { eAuthority::Makers, "Makers" },
    { eAuthority::MakerSymK, "MakerSymK" },
    { eAuthority::MakerPuK, "MakerPuK" },
    { eAuthority::SID, "SID" },
    { eAuthority::TPerSign, "TPerSign" },
    { eAuthority::TPerExch, "TPerExch" },
    { eAuthority::AdminExch, "AdminExch" },
    { eAuthority::Issuers, "Issuers" },
    { eAuthority::Editors, "Editors" },
    { eAuthority::Deleters, "Deleters" },
    { eAuthority::Servers, "Servers" },
    { opal::eAuthority::PSID, "PSID" },
    { opal::eAuthority::Admin1, "Admin1" },
    { opal::eAuthority::Admin2, "Admin2" },
    { opal::eAuthority::Admin3, "Admin3" },
    { opal::eAuthority::Admin4, "Admin4" },
    { opal::eAuthority::Users, "Users" },
    { opal::eAuthority::User1, "User1" },
    { opal::eAuthority::User2, "User2" },
    { opal::eAuthority::User3, "User3" },
    { opal::eAuthority::User4, "User4" },
    { opal::eAuthority::User5, "User5" },
    { opal::eAuthority::User6, "User6" },
    { opal::eAuthority::User7, "User7" },
    { opal::eAuthority::User8, "User8" },
    { opal::eAuthority::User9, "User9" },
    { opal::eAuthority::User10, "User10" },
    { opal::eAuthority::User11, "User11" },
    { opal::eAuthority::User12, "User12" },
};


std::optional<std::string_view> GetName(Uid uid) {
    static const std::unordered_map<uint64_t, std::string_view> mapping = [] {
        std::unordered_map<uint64_t, std::string_view> mapping;
        for (const auto& [uid, name] : securityProviders) {
            mapping.insert({ uint64_t(uid), name });
        }
        for (const auto& [uid, name] : authorities) {
            mapping.insert({ uint64_t(uid), name });
        }
        return mapping;
    }();
    const auto it = mapping.find(uint64_t(uid));
    if (it != mapping.end()) {
        return it->second;
    }
    return std::nullopt;
}


std::string GetNameOrUid(Uid uid) {
    std::string_view properName = GetName(uid).value_or(std::string_view{});
    return !properName.empty() ? std::string(properName) : std::format("{:#018x}", uint64_t(uid));
}


std::vector<Uid> GetUid(std::string_view name) {
    static const std::unordered_multimap<std::string_view, Uid> mapping = [] {
        std::unordered_multimap<std::string_view, Uid> mapping;
        for (const auto& [uid, name] : securityProviders) {
            mapping.insert({ name, uid });
        }
        for (const auto& [uid, name] : authorities) {
            mapping.insert({ name, uid });
        }
        return mapping;
    }();
    const auto [first, last] = mapping.equal_range(name);
    std::vector<Uid> matches;
    std::transform(first, last, std::back_inserter(matches), [](const auto& v) {
        return v.second;
    });
    return matches;
}