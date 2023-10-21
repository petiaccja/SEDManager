#pragma once

#include <Data/NativeTypes.hpp>

#include <SEDManager/SEDManager.hpp>

#include <bit>
#include <string_view>
#include <vector>


std::vector<std::byte> GetPassword(std::string_view prompt);

std::optional<Uid> FindOrParseUid(SEDManager& app, std::string_view nameOrUid);

std::vector<std::string_view> SplitName(std::string_view name);

std::string FormatTable(std::span<const std::string> columns, std::span<const std::vector<std::string>> rows);


template <class Range>
std::string Join(Range&& r, std::string_view sep) {
    std::stringstream ss;
    for (auto it = std::begin(r); it != std::end(r); ++it) {
        ss << *it;
        auto copy = it;
        std::advance(copy, 1);
        if (copy != std::end(r)) {
            ss << sep;
        }
    }
    return ss.str();
}