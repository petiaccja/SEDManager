#pragma once

#include <Data/NativeTypes.hpp>

#include <SEDManager/SEDManager.hpp>

#include <bit>
#include <optional>
#include <string_view>
#include <vector>


std::vector<std::byte> GetPassword(std::string_view prompt);

std::optional<Uid> FindOrParseUid(SEDManager& app, std::string_view nameOrUid, std::optional<Uid> sp = {});

std::vector<std::string_view> SplitName(std::string_view name);

std::string FormatTable(std::span<const std::string> columns, std::span<const std::vector<std::string>> rows);

template <class T>
const T& Unwrap(const std::optional<T>& maybeValue, std::string_view message = {}) {
    if (!maybeValue) {
        throw std::logic_error(std::string(message));
    }
    return *maybeValue;
}


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