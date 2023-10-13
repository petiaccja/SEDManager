#pragma once

#include <stdexcept>


struct PasswordError : std::logic_error {
    PasswordError() : std::logic_error("invalid password") {}
};


struct NotImplementedError : std::logic_error {
    NotImplementedError(std::string message) : std::logic_error(std::move(message)) {}
};