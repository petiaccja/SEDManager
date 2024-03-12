#pragma once

#include <Messaging/Native.hpp>

#include <EncryptedDevice/EncryptedDevice.hpp>

#include <optional>
#include <vector>


std::vector<std::byte> GetPassword(std::string_view prompt);
std::string GetMultiline(std::string_view terminator);

std::optional<sedmgr::UID> ParseObjectRef(const sedmgr::ModuleCollection& app, std::string_view nameOrUid, std::optional<sedmgr::UID> sp = {});
std::string FormatObjectRef(const sedmgr::ModuleCollection& app, sedmgr::UID uid, std::optional<sedmgr::UID> sp = {});
std::string FormatTable(std::span<const std::string> columns, std::span<const std::vector<std::string>> rows);

std::vector<std::string_view> SplitName(std::string_view name);


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