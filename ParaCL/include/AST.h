#pragma once

#include "concepts.h"
#include "symbol_table.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using AST::symbol_table_t;
using IIterator = typename symbol_table_t::iterator;
using ipcl_val = typename std::variant<int, IIterator>;

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
    const node_types nt;
    ast_node_t(node_types n_t) : nt(n_t) {}
    virtual ipcl_val Iprocess(const symbol_table_t &) const { return {}; }
    virtual ipcl_val Iprocess(symbol_table_t &) const { return {}; }
    virtual ~ast_node_t() = default;
};

struct ast_expr_t : public ast_node_t {
    ast_expr_t(node_types n_t) : ast_node_t(n_t) {}
    virtual ~ast_expr_t() = default;
};

struct ast_num_t final : public ast_expr_t {
    int val;

    ipcl_val Iprocess(const symbol_table_t &) const override { return val; }
    ast_num_t(int vall) noexcept : ast_expr_t(node_types::NUMBER), val(vall) {}
};

struct ast_var_t : public ast_expr_t {
    std::string name;

    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        auto var_val = st.find(name)->second;
        return ipcl_val{var_val};
    }
    ast_var_t(std::string_view namee, node_types n_t = node_types::VARIABLE)
        : ast_expr_t(n_t), name(namee)
    {}
    virtual ~ast_var_t() = default;
};

struct ast_empty_op_t final : public ast_expr_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override { return {}; }
    ast_empty_op_t() : ast_expr_t(node_types::EMPTY) {}
};

struct ast_lval_t : public ast_var_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        IIterator res = st.add_name(name);
        assert(res != st.end());
        return ipcl_val{res};
    }

    ast_lval_t(std::string_view namee) : ast_var_t(namee, node_types::LVAL) {}
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
    std::shared_ptr<ast_expr_t> lhs, rhs;

    ast_bin_op_t(ast_bin_ops opp, std::shared_ptr<ast_expr_t> lhss,
                 std::shared_ptr<ast_expr_t> rhss)
        : ast_expr_t(node_types::BIN_OP), op(opp), lhs(std::move(lhss)),
          rhs(std::move(rhss))
    {}
    ast_bin_op_t(ast_bin_ops opp, std::shared_ptr<ast_expr_t> rhss)
        : ast_expr_t(node_types::BIN_OP), op(opp), rhs(std::move(rhss))
    {}

    virtual constexpr std::string_view op_str() const = 0;
    virtual ~ast_bin_op_t() = default;
};

struct ast_plus_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) +
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "+"; }

    ast_plus_op(std::shared_ptr<ast_expr_t> lhss,
                std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::PLUS, lhss, rhss)
    {}
};

struct ast_minus_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) -
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "-"; }

    ast_minus_op(std::shared_ptr<ast_expr_t> lhss,
                 std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::MINUS, lhss, rhss)
    {}
};

struct ast_mul_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) *
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "*"; }

    ast_mul_op(std::shared_ptr<ast_expr_t> lhss,
               std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::MULTIPLICATION, lhss, rhss)
    {}
};

struct ast_div_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) /
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "/"; }

    ast_div_op(std::shared_ptr<ast_expr_t> lhss,
               std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::DIVISION, lhss, rhss)
    {}
};

struct ast_assign_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        int val = std::get<int>(rhs->Iprocess(st));
        std::get<IIterator>(lhs->Iprocess(const_cast<symbol_table_t &>(st)))
            ->second = val;
        return {val};
    }
    constexpr std::string_view op_str() const override { return "="; }

    ast_assign_op(std::shared_ptr<ast_expr_t> lhss,
                  std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::ASSIGNMENT, lhss, rhss)
    {}
    ast_assign_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::ASSIGNMENT, rhss)
    {}
    ast_assign_op(std::shared_ptr<ast_lval_t> lhss,
                  std::shared_ptr<ast_assign_op> other)
        : ast_bin_op_t(*other)
    {
        lhs = lhss;
    }
};

struct ast_greater_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) >
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return ">"; }

    ast_greater_op(std::shared_ptr<ast_expr_t> lhss,
                   std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::GREATER, lhss, rhss)
    {}
};

struct ast_less_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) <
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "<"; }

    ast_less_op(std::shared_ptr<ast_expr_t> lhss,
                std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::LESS, lhss, rhss)
    {}
};

struct ast_greatereq_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) >=
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return ">="; }

    ast_greatereq_op(std::shared_ptr<ast_expr_t> lhss,
                     std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::GREATEREQ, lhss, rhss)
    {}
};

struct ast_lesseq_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) <=
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "<="; }

    ast_lesseq_op(std::shared_ptr<ast_expr_t> lhss,
                  std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::LESSEQ, lhss, rhss)
    {}
};

struct ast_equal_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) ==
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "=="; }

    ast_equal_op(std::shared_ptr<ast_expr_t> lhss,
                 std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::EQUAL, lhss, rhss)
    {}
};

struct ast_notequal_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) !=
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "!="; }

    ast_notequal_op(std::shared_ptr<ast_expr_t> lhss,
                    std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::NOTEQUAL, lhss, rhss)
    {}
};

struct ast_logical_and_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) &&
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "&&"; }

    ast_logical_and_op(std::shared_ptr<ast_expr_t> lhss,
                       std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::LAND, lhss, rhss)
    {}
};

struct ast_logical_or_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) ||
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "||"; }

    ast_logical_or_op(std::shared_ptr<ast_expr_t> lhss,
                      std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::LOR, lhss, rhss)
    {}
};

struct ast_modular_division_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) %
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr std::string_view op_str() const override { return "%"; }

    ast_modular_division_op(std::shared_ptr<ast_expr_t> lhss,
                            std::shared_ptr<ast_expr_t> rhss)
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
    std::shared_ptr<ast_expr_t> rhs;

    ast_un_op_t(ast_un_ops opp, std::shared_ptr<ast_expr_t> rhss)
        : ast_expr_t(node_types::UN_OP), op(opp), rhs(rhss)
    {}

    virtual constexpr std::string_view op_str() const = 0;
    virtual ~ast_un_op_t() = default;
};

struct ast_print_op final : public ast_un_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        int val = std::get<int>(rhs->Iprocess(st));
        std::cout << val << std::endl;
        return val;
    }
    constexpr std::string_view op_str() const override { return "print"; }

    ast_print_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_un_op_t(ast_un_ops::PRINT, rhss)
    {}
};

struct ast_unminus_op final : public ast_un_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return -std::get<int>(rhs->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "-"; }

    ast_unminus_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_un_op_t(ast_un_ops::MINUS, rhss)
    {}
};

struct ast_unplus_op final : public ast_un_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return std::get<int>(rhs->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "+"; }

    ast_unplus_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_un_op_t(ast_un_ops::PLUS, rhss)
    {}
};

struct ast_logical_no_op final : public ast_un_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return !std::get<int>(rhs->Iprocess(st));
    }
    constexpr std::string_view op_str() const override { return "!"; }

    ast_logical_no_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_un_op_t(ast_un_ops::LNO, rhss)
    {}
};

struct ast_write_t final : public ast_expr_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        int tmp;
        std::cin >> tmp;
        return {tmp};
    }
    ast_write_t() : ast_expr_t(node_types::WRITE) {}
};

struct ast_statements_t : public ast_node_t {
    using deque_t = std::deque<std::shared_ptr<ast_node_t>>;
    deque_t seq;

    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return process_sequency(st);
    }
    ast_statements_t(node_types n_t = node_types::STATEMENTS) : ast_node_t(n_t)
    {}
    ast_statements_t(std::shared_ptr<ast_node_t> expr,
                     std::shared_ptr<ast_statements_t> other,
                     node_types n_t = node_types::STATEMENTS)
        : ast_node_t(n_t), seq(other ? std::move(other->seq) : deque_t())
    {
        if (expr)
            seq.emplace_front(std::move(expr));
    }

    virtual ~ast_statements_t() = default;

    ipcl_val process_sequency(const symbol_table_t &st) const
    {
        ipcl_val res{};
        for (auto &&p : seq)
        {
            res = p->Iprocess(st);
        }
        return res;
    }
};

struct ast_scope_t final : public ast_statements_t {
    ast_scope_t(std::shared_ptr<ast_statements_t> stmts)
        : ast_statements_t(*stmts)
    {}

    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        const_cast<symbol_table_t &>(st).emplace_scope();
        auto res = process_sequency(st);
        const_cast<symbol_table_t &>(st).pop_scope();
        return res;
    }
};

struct ast_if_t : public ast_node_t {
    std::shared_ptr<ast_expr_t> condition;
    std::shared_ptr<ast_node_t> body;

    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        if (std::get<int>(condition->Iprocess(st)))
            return body->Iprocess(st);
        return {};
    }
    ast_if_t(std::shared_ptr<ast_expr_t> cond, std::shared_ptr<ast_node_t> bod,
             node_types n_t = node_types::IF)
        : ast_node_t(n_t), condition(cond), body(bod)
    {}
    virtual ~ast_if_t() = default;
};

struct ast_ifelse_t final : public ast_if_t {
    std::shared_ptr<ast_node_t> else_body;

    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        if (std::get<int>(condition->Iprocess(st)))
            return body->Iprocess(st);
        return else_body->Iprocess(st);
    }
    ast_ifelse_t(std::shared_ptr<ast_if_t> ifst,
                 std::shared_ptr<ast_node_t> else_bod)
        : ast_if_t(ifst->condition, ifst->body, node_types::IFELSE),
          else_body(else_bod)
    {}
};

struct ast_while_t final : public ast_node_t {
    std::shared_ptr<ast_expr_t> condition;
    std::shared_ptr<ast_node_t> body;

    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        ipcl_val res;
        while (std::get<int>(condition->Iprocess(st)))
            res = body->Iprocess(st);
        return res;
    }
    ast_while_t(std::shared_ptr<ast_expr_t> cond,
                std::shared_ptr<ast_node_t> bod)
        : ast_node_t(node_types::WHILE), condition(cond), body(bod)
    {}
};

class IIast_t {
public:
    virtual const ast_node_t &root() const = 0;
    virtual int execute(const symbol_table_t &st) const = 0;
    virtual ~IIast_t() = default;
};

class ast_t final : public IIast_t {
    std::shared_ptr<ast_node_t> root_;

public:
    using node_ptr = typename std::shared_ptr<ast_node_t>;

    ast_t() noexcept {}

    const ast_node_t &root() const override { return *root_; }
    int execute(const symbol_table_t &st) const override
    {
        root_->Iprocess(st);
        return 0;
    }

    void set_root(node_ptr root) { root_ = std::move(root); }
};

template <smart_pointer T, class... Args> T make_node(Args &&... args)
{
    return std::make_shared<typename T::element_type>(
        std::forward<Args>(args)...);
}

template <smart_pointer T, typename SrcT>
T node_cast(const std::shared_ptr<SrcT> &r)
{
    return static_pointer_cast<typename T::element_type>(r);
}

template <smart_pointer T, typename SrcT> T node_cast(std::shared_ptr<SrcT> &&r)
{
    return static_pointer_cast<typename T::element_type>(r);
}

} // namespace AST