#pragma once

#include "UID.hpp"

#include <compare>
#include <cstdint>
#include <optional>
#include <variant>


namespace sedmgr {

struct CellBlock {
    std::optional<std::variant<UID, uint32_t>> startRow;
    std::optional<uint32_t> endRow;
    std::optional<uint32_t> startColumn;
    std::optional<uint32_t> endColumn;

    std::strong_ordering operator<=>(const CellBlock&) const noexcept = default;
};

} // namespace sedmgr
