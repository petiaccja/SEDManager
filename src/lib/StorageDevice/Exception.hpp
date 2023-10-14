#pragma once

#include <stdexcept>


struct DeviceError : std::runtime_error {
    DeviceError(std::string message) : std::runtime_error(std::move(message)) {}
};