#pragma once

#include "symbol_table.h"

#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace AST {

using IIterator = typename symbol_table_t::iterator;
using ipcl_val = typename std::variant<int, IIterator>;
template <typename T>
concept sem_t = (std::is_same_v<T, int> || std::is_same_v<T, IIterator>);

template <sem_t T, sem_t U, bool = true> ipcl_val operator+(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator-(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator*(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator/(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val assign(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <bool = true> ipcl_val assign(IIterator it, int rhs)
{
    return it->second = rhs;
}
template <sem_t T, sem_t U> ipcl_val operator<(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator>(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator<=(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator>=(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U,
          std::enable_if_t<!std::is_same_v<T, U>, bool> = true>
ipcl_val operator==(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U,
          std::enable_if_t<!std::is_same_v<T, U>, bool> = true>
ipcl_val operator!=(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator&&(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator||(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T, sem_t U> ipcl_val operator%(T, U)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T> std::ostream operator<<(std::ostream &, T)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T> ipcl_val operator-(T)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
template <sem_t T> ipcl_val operator!(T)
{
    throw std::invalid_argument("Invalid argument of operator.");
}
struct ast_cond_visitor final {
    bool operator()(int el) const { return el; }
    bool operator()(IIterator el) const
    {
        throw std::invalid_argument("Invalid argument of operator.");
    }
};

} // namespace AST