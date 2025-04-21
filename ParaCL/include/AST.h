#pragma once

#include "concepts.h"
#include "driver_exceptions.h"
#include "semantic.h"
#include "symbol_table.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace AST {

enum class node_types {
    NUMBER,
    VARIABLE,
    BIN_OP,
    UN_OP,
    STATEMENTS,
    WRITE,
    LVAL,
    IF,
    IFELSE,
    WHILE,
    EMPTY,
};

struct ast_node_t {
    using node_ptr = std::shared_ptr<ast_node_t>;
    using node_it = std::list<node_ptr>::iterator;
    const node_types nt;
    ast_node_t(node_types n_t) : nt(n_t) {}
    virtual ipcl_val Iprocess(symbol_table_t &) const { return {}; }
    virtual ~ast_node_t() = default;
};

struct ast_expr_t : public ast_node_t {
    ast_expr_t(node_types n_t) : ast_node_t(n_t) {}
    virtual ~ast_expr_t() = default;
};

struct ast_num_t final : public ast_expr_t {
    int val;

    ipcl_val Iprocess(symbol_table_t &) const override { return val; }
    ast_num_t(int vall) noexcept : ast_expr_t(node_types::NUMBER), val(vall) {}
};

struct ast_var_t : public ast_expr_t {
    std::string name;

    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        auto var_val = st.find(name)->second;
        return ipcl_val{var_val};
    }
    ast_var_t(std::string_view namee, symbol_table_t &st,
              node_types n_t = node_types::VARIABLE)
        : ast_expr_t(n_t), name(namee)
    {
        if (st.find(std::string{name}) == st.cend())
            throw ExceptsPCL::compilation_error("Undefined variable: " +
                                                std::string(name));
    }
    ast_var_t(std::string_view namee, node_types n_t = node_types::VARIABLE)
        : ast_expr_t(n_t), name(namee)
    {}
    virtual ~ast_var_t() = default;
};

struct ast_empty_op_t final : public ast_expr_t {
    ipcl_val Iprocess(symbol_table_t &) const override { return {}; }
    ast_empty_op_t() : ast_expr_t(node_types::EMPTY) {}
};

struct ast_lval_t : public ast_var_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        IIterator res = st.add_name(name);
        assert(res != st.end());
        return ipcl_val{res};
    }

    ast_lval_t(std::string_view namee, symbol_table_t &st)
        : ast_var_t(namee, node_types::LVAL)
    {
        st.add_name(name);
    }
    virtual ~ast_lval_t() = default;
};

enum class ast_bin_ops {
    PLUS,
    MINUS,
    MULTIPLICATION,
    DIVISION,
    ASSIGNMENT,
    GREATER,
    LESS,
    GREATEREQ,
    LESSEQ,
    EQUAL,
    NOTEQUAL,
    LAND,
    LOR,
    MODDIV
};

struct ast_bin_op_t : public ast_expr_t {
    ast_bin_ops op;
    node_it lhs, rhs;

    ast_bin_op_t(ast_bin_ops opp, node_it lhss, node_it rhss)
        : ast_expr_t(node_types::BIN_OP), op(opp), lhs(lhss), rhs(rhss)
    {}
    ast_bin_op_t(ast_bin_ops opp, node_it rhss)
        : ast_expr_t(node_types::BIN_OP), op(opp), rhs(rhss)
    {}

    virtual constexpr std::string_view op_str() const = 0;
    virtual ~ast_bin_op_t() = default;
};

struct ast_plus_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs + rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "+"; }

    ast_plus_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::PLUS, lhss, rhss)
    {}
};

struct ast_minus_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs - rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "-"; }

    ast_minus_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::MINUS, lhss, rhss)
    {}
};

struct ast_mul_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs * rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "*"; }

    ast_mul_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::MULTIPLICATION, lhss, rhss)
    {}
};

struct ast_div_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs / rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "/"; }

    ast_div_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::DIVISION, lhss, rhss)
    {}
};

struct ast_assign_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return assign(lhs, rhs); },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "="; }

    ast_assign_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::ASSIGNMENT, lhss, rhss)
    {}
    ast_assign_op(node_it rhss) : ast_bin_op_t(ast_bin_ops::ASSIGNMENT, rhss) {}
};

struct ast_greater_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs > rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return ">"; }

    ast_greater_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::GREATER, lhss, rhss)
    {}
};

struct ast_less_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs < rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "<"; }

    ast_less_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::LESS, lhss, rhss)
    {}
};

struct ast_greatereq_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs >= rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return ">="; }

    ast_greatereq_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::GREATEREQ, lhss, rhss)
    {}
};

struct ast_lesseq_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs <= rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "<="; }

    ast_lesseq_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::LESSEQ, lhss, rhss)
    {}
};

struct ast_equal_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs == rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "=="; }

    ast_equal_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::EQUAL, lhss, rhss)
    {}
};

struct ast_notequal_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs != rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "!="; }

    ast_notequal_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::NOTEQUAL, lhss, rhss)
    {}
};

struct ast_logical_and_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs && rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "&&"; }

    ast_logical_and_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::LAND, lhss, rhss)
    {}
};

struct ast_logical_or_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs || rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "||"; }

    ast_logical_or_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::LOR, lhss, rhss)
    {}
};

struct ast_modular_division_op final : public ast_bin_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&lhs, auto &&rhs) -> ipcl_val { return lhs % rhs; },
            (*lhs)->Iprocess(st), (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "%"; }

    ast_modular_division_op(node_it lhss, node_it rhss)
        : ast_bin_op_t(ast_bin_ops::MODDIV, lhss, rhss)
    {}
};

enum class ast_un_ops {
    PRINT,
    MINUS,
    PLUS,
    LNO,
};

struct ast_un_op_t : public ast_expr_t {
    ast_un_ops op;
    node_it rhs;

    ast_un_op_t(ast_un_ops opp, node_it rhss)
        : ast_expr_t(node_types::UN_OP), op(opp), rhs(rhss)
    {}

    virtual constexpr std::string_view op_str() const = 0;
    virtual ~ast_un_op_t() = default;
};

struct ast_print_op final : public ast_un_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit(
            [](auto &&el) -> ipcl_val {
                std::cout << el << std::endl;
                return el;
            },
            (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "print"; }

    ast_print_op(node_it rhss) : ast_un_op_t(ast_un_ops::PRINT, rhss) {}
};

struct ast_unminus_op final : public ast_un_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit([](auto &&el) -> ipcl_val { return -el; },
                          (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "-"; }

    ast_unminus_op(node_it rhss) : ast_un_op_t(ast_un_ops::MINUS, rhss) {}
};

struct ast_unplus_op final : public ast_un_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit([](auto &&el) -> ipcl_val { return el; },
                          (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "+"; }

    ast_unplus_op(node_it rhss) : ast_un_op_t(ast_un_ops::PLUS, rhss) {}
};

struct ast_logical_no_op final : public ast_un_op_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return std::visit([](auto &&el) -> ipcl_val { return !el; },
                          (*rhs)->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "!"; }

    ast_logical_no_op(node_it rhss) : ast_un_op_t(ast_un_ops::LNO, rhss) {}
};

struct ast_write_t final : public ast_expr_t {
    ipcl_val Iprocess(symbol_table_t &) const override
    {
        int tmp;
        std::cin >> tmp;
        return {tmp};
    }
    ast_write_t() : ast_expr_t(node_types::WRITE) {}
};

struct ast_statements_t : public ast_node_t {
    using deque_t = std::deque<node_it>;
    deque_t seq;

    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        return process_sequency(st);
    }
    ast_statements_t() : ast_node_t(node_types::STATEMENTS) {}
    ast_statements_t(node_it other)
        : ast_node_t(node_types::STATEMENTS),
          seq(std::move(static_pointer_cast<ast_statements_t>(*other)->seq))
    {}
    ast_statements_t(node_it expr, node_it other)
        : ast_node_t(node_types::STATEMENTS),
          seq(*other ? std::move(
                           static_pointer_cast<ast_statements_t>(*other)->seq)
                     : deque_t())
    {
        if (*expr)
            seq.emplace_front(expr);
    }

    virtual ~ast_statements_t() = default;

    ipcl_val process_sequency(symbol_table_t &st) const
    {
        ipcl_val res{};
        for (auto &&p : seq)
        {
            res = (*p)->Iprocess(st);
        }
        return res;
    }
};

struct ast_scope_t final : public ast_statements_t {
    ast_scope_t(node_it stmts) : ast_statements_t(stmts) {}

    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        st.emplace_scope();
        auto res = process_sequency(st);
        st.pop_scope();
        return res;
    }
};

struct ast_if_t : public ast_node_t {
    node_it condition;
    node_it body;

    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        if (std::visit(ast_cond_visitor{}, (*condition)->Iprocess(st)))
            return (*body)->Iprocess(st);
        return {};
    }
    ast_if_t(node_it cond, node_it bod, node_types n_t = node_types::IF)
        : ast_node_t(n_t), condition(cond), body(bod)
    {}
    virtual ~ast_if_t() = default;
};

struct ast_ifelse_t final : public ast_if_t {
    node_it else_body;

    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        if (std::visit(ast_cond_visitor{}, (*condition)->Iprocess(st)))
            return (*body)->Iprocess(st);
        return (*else_body)->Iprocess(st);
    }
    ast_ifelse_t(node_it ifst, node_it else_bod)
        : ast_if_t(static_pointer_cast<ast_if_t>(*ifst)->condition,
                   static_pointer_cast<ast_if_t>(*ifst)->body,
                   node_types::IFELSE),
          else_body(else_bod)
    {}
};

struct ast_while_t final : public ast_node_t {
    node_it condition;
    node_it body;

    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        ipcl_val res;
        while (std::visit(ast_cond_visitor{}, (*condition)->Iprocess(st)))
            res = (*body)->Iprocess(st);
        return res;
    }
    ast_while_t(node_it cond, node_it bod)
        : ast_node_t(node_types::WHILE), condition(cond), body(bod)
    {}
};

class IIast_t {
public:
    virtual const ast_node_t &root() const = 0;
    virtual int execute(symbol_table_t &) const = 0;
    virtual ~IIast_t() = default;
};

class ast_t final : public IIast_t {
public:
    using node_it = std::list<std::shared_ptr<ast_node_t>>::iterator;
    using node_ptr = typename std::shared_ptr<ast_node_t>;

private:
    node_it root_;
    std::list<node_ptr> nodes_;

public:
    ast_t() noexcept {}

    const ast_node_t &root() const override { return **root_; }
    int execute(symbol_table_t &st) const override
    {
        (*root_)->Iprocess(st);
        return 0;
    }

    void set_root(node_it root) { root_ = root; }

    template <typename T, class... Args> node_it make_node(Args &&... args)
    {
        nodes_.push_back(std::make_shared<T>(std::forward<Args>(args)...));
        return std::prev(nodes_.end());
    }
};

} // namespace AST