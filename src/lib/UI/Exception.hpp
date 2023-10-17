#pragma once

#include <stdexcept>
#include <format>


struct TypeError : std::logic_error {
    TypeError(std::string_view expected, std::string_view actual)
        : std::logic_error(std::format("expected a value of '{}' but got a value of '{}'", expected, actual)) {}
};
