#pragma once

#include <stdexcept>

namespace ExceptsPCL {

class paracl_error : public std::runtime_error {
public:
    paracl_error(const std::string &what_arg) : std::runtime_error(what_arg) {}
    virtual ~paracl_error() = default;
};

class compilation_error final : public paracl_error {
public:
    compilation_error(const std::string &what_arg) : paracl_error(what_arg) {}
};

}; // namespace ExceptsPCL