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
};

struct IIast_node_t {
    virtual ipcl_val Iprocess(const symbol_table_t &) const { return {}; }
    virtual ipcl_val Iprocess(symbol_table_t &) const { return {}; }
    virtual ~IIast_node_t() = default;
};

struct ast_node_t : public IIast_node_t {
    virtual constexpr node_types get_type() const = 0;
    virtual ~ast_node_t() = default;
};

struct ast_expr_t : public ast_node_t {
    virtual ~ast_expr_t() = default;
};

struct ast_num_t final : public ast_expr_t {
    int val;

    ipcl_val Iprocess(const symbol_table_t &) const override { return val; }
    constexpr node_types get_type() const override
    {
        return node_types::NUMBER;
    }
    ast_num_t(int vall) noexcept : val(vall) {}
};

struct ast_var_t : public ast_expr_t {
    std::string name;

    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        auto var_val = st.find(name)->second;
        return ipcl_val{var_val.get()};
    }
    constexpr node_types get_type() const override
    {
        return node_types::VARIABLE;
    }
    ast_var_t(const std::string &namee) : name(namee) {}
    virtual ~ast_var_t() = default;
};

struct ast_lval_t : public ast_var_t {
    ipcl_val Iprocess(symbol_table_t &st) const override
    {
        IIterator res = st.find(name);
        return ipcl_val{res};
    }
    constexpr node_types get_type() const override { return node_types::LVAL; }

    ast_lval_t(const std::string &namee) : ast_var_t(namee) {}
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
};

struct ast_bin_op_t : public ast_expr_t {
    ast_bin_ops op;
    std::shared_ptr<ast_expr_t> lhs, rhs;

    constexpr node_types get_type() const override
    {
        return node_types::BIN_OP;
    }
    ast_bin_op_t(ast_bin_ops opp, std::shared_ptr<ast_expr_t> lhss,
                 std::shared_ptr<ast_expr_t> rhss)
        : op(opp), lhs(std::move(lhss)), rhs(std::move(rhss))
    {}

    ast_bin_op_t(ast_bin_ops opp, std::shared_ptr<ast_expr_t> rhss)
        : op(opp), rhs(std::move(rhss))
    {}

    virtual constexpr const char *op_str() const = 0;
    virtual ~ast_bin_op_t() = default;
};

struct ast_plus_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) +
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return "+"; }

    ast_plus_op(std::shared_ptr<ast_expr_t> lhss,
                std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::PLUS, lhss, rhss)
    {}
    ast_plus_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::PLUS, rhss)
    {}
};

struct ast_minus_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) -
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return "-"; }

    ast_minus_op(std::shared_ptr<ast_expr_t> lhss,
                 std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::MINUS, lhss, rhss)
    {}
    ast_minus_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::MINUS, rhss)
    {}
};

struct ast_mul_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) *
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return "*"; }

    ast_mul_op(std::shared_ptr<ast_expr_t> lhss,
               std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::MULTIPLICATION, lhss, rhss)
    {}
    ast_mul_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::MULTIPLICATION, rhss)
    {}
};

struct ast_div_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) /
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return "/"; }

    ast_div_op(std::shared_ptr<ast_expr_t> lhss,
               std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::DIVISION, lhss, rhss)
    {}
    ast_div_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::DIVISION, rhss)
    {}
};

struct ast_assign_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        int val = std::get<int>(rhs->Iprocess(st));
        std::get<IIterator>(lhs->Iprocess(const_cast<symbol_table_t &>(st)))
            ->second.set(val);
        return {val};
    }
    constexpr const char *op_str() const override { return "="; }

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
    constexpr const char *op_str() const override { return ">"; }

    ast_greater_op(std::shared_ptr<ast_expr_t> lhss,
                   std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::GREATER, lhss, rhss)
    {}
    ast_greater_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::GREATER, rhss)
    {}
};

struct ast_less_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) <
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return "<"; }

    ast_less_op(std::shared_ptr<ast_expr_t> lhss,
                std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::LESS, lhss, rhss)
    {}
    ast_less_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::LESS, rhss)
    {}
};

struct ast_greatereq_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) >=
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return ">="; }

    ast_greatereq_op(std::shared_ptr<ast_expr_t> lhss,
                     std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::GREATEREQ, lhss, rhss)
    {}
    ast_greatereq_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::GREATEREQ, rhss)
    {}
};

struct ast_lesseq_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) <=
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return "<="; }

    ast_lesseq_op(std::shared_ptr<ast_expr_t> lhss,
                  std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::LESSEQ, lhss, rhss)
    {}
    ast_lesseq_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::LESSEQ, rhss)
    {}
};

struct ast_equal_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) ==
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return "=="; }

    ast_equal_op(std::shared_ptr<ast_expr_t> lhss,
                 std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::EQUAL, lhss, rhss)
    {}
    ast_equal_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::EQUAL, rhss)
    {}
};

struct ast_notequal_op final : public ast_bin_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        return {std::get<int>(lhs->Iprocess(st)) !=
                std::get<int>(rhs->Iprocess(st))};
    }
    constexpr const char *op_str() const override { return "!="; }

    ast_notequal_op(std::shared_ptr<ast_expr_t> lhss,
                    std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::NOTEQUAL, lhss, rhss)
    {}
    ast_notequal_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_bin_op_t(ast_bin_ops::NOTEQUAL, rhss)
    {}
};

enum class ast_un_ops {
    PRINT,
};

struct ast_un_op_t : public ast_expr_t {
    ast_un_ops op;
    std::shared_ptr<ast_expr_t> rhs;

    constexpr node_types get_type() const override { return node_types::UN_OP; }
    ast_un_op_t(ast_un_ops opp, std::shared_ptr<ast_expr_t> rhss)
        : op(opp), rhs(rhss)
    {}

    virtual constexpr const char *op_str() const = 0;
    virtual ~ast_un_op_t() = default;
};

struct ast_print_op final : public ast_un_op_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        int val = std::get<int>(rhs->Iprocess(st));
        std::cout << val << std::endl;
        return val;
    }
    constexpr const char *op_str() const override { return "print"; }

    ast_print_op(std::shared_ptr<ast_expr_t> rhss)
        : ast_un_op_t(ast_un_ops::PRINT, rhss)
    {}
};

struct ast_write_t final : public ast_expr_t {
    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        int tmp;
        std::cin >> tmp;
        return {tmp};
    }
    constexpr node_types get_type() const override { return node_types::WRITE; }
};

struct ast_statements_t final : public ast_node_t {
    using deque_t = std::deque<std::shared_ptr<ast_node_t>>;
    deque_t seq;

    ipcl_val Iprocess(const symbol_table_t &st) const override
    {
        ipcl_val res;
        for (auto &&it : seq)
            res = it->Iprocess(st);
        return res;
    }
    constexpr node_types get_type() const override
    {
        return node_types::STATEMENTS;
    }
    ast_statements_t() {}
    ast_statements_t(std::shared_ptr<ast_node_t> expr,
                     std::shared_ptr<ast_statements_t> other)
        : seq(other ? std::move(other->seq) : deque_t())
    {
        seq.emplace_front(std::move(expr));
    }
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

class ast_node_dumper final {
    std::ostream *debug_stream_;

private:
    std::string get_label_str(const ast_node_t &node) const
    {
        switch (node.get_type())
        {
        case node_types::NUMBER:
            return "Number\\n\\l " +
                   std::to_string(static_cast<const ast_num_t &>(node).val) +
                   " \\l";
            break;
        case node_types::VARIABLE:
            return "Variable\\n\\l " +
                   static_cast<const ast_var_t &>(node).name + " \\l";
            break;
        case node_types::BIN_OP:
            return static_cast<const ast_bin_op_t &>(node).op_str();
            break;
        case node_types::UN_OP:
            return static_cast<const ast_un_op_t &>(node).op_str();
            break;
        case node_types::STATEMENTS:
            return "Statements";
            break;
        case node_types::WRITE:
            return "Write";
            break;
        case node_types::LVAL:
            return "Left value\\n\\l " +
                   static_cast<const ast_var_t &>(node).name + " \\l";
            break;
        default:
            assert(0 && "Unreachable.");
            break;
        }
    }

public:
    ast_node_dumper(std::ostream *ds) : debug_stream_(ds) {}

    void operator()(const ast_node_t &node, int node_id) const
    {
        *debug_stream_ << "\t" << node_id << " [label=\"" << get_label_str(node)
                       << "\"]\n";
    }
};

struct dot_edge_t final {
    std::pair<int, int> line;
    std::string label;
    dot_edge_t(int src, int dst, const std::string &labell = "")
        : line(src, dst), label(labell)
    {}
};

class dot_ast_t final {
    const IIast_t *ast_;
    std::unordered_map<int, const ast_node_t *> nodes_;
    std::vector<dot_edge_t> edges_;
    int ids = 1;

private:
    void add_node(const ast_node_t &node, int id = 0)
    {
        switch (node.get_type())
        {
        case node_types::NUMBER:
            nodes_.try_emplace(id, &node);
            break;
        case node_types::VARIABLE:
            nodes_.try_emplace(id, &node);
            break;
        case node_types::BIN_OP:
        {
            int l_id = ids++, r_id = ids++;
            nodes_.try_emplace(id, &node);
            edges_.push_back({id, l_id, "lhs"});
            edges_.push_back({id, r_id, "rhs"});
            add_node(*(static_cast<const ast_bin_op_t &>(node).lhs), l_id);
            add_node(*(static_cast<const ast_bin_op_t &>(node).rhs), r_id);
            break;
        }
        case node_types::UN_OP:
        {
            int r_id = ids++;
            nodes_.try_emplace(id, &node);
            edges_.push_back({id, r_id, "operand"});
            add_node(*(static_cast<const ast_un_op_t &>(node).rhs), r_id);
            break;
        }
        case node_types::STATEMENTS:
        {
            for (auto &&it : static_cast<const ast_statements_t &>(node).seq)
            {
                int r_id = ids++;
                nodes_.try_emplace(id, &node);
                edges_.push_back({id, r_id});
                add_node(*it, r_id);
            }
            break;
        }
        case node_types::WRITE:
            nodes_.try_emplace(id, &node);
            break;
        case node_types::LVAL:
            nodes_.try_emplace(id, &node);
            break;
        default:
            assert(0 && "Unreachable.");
            break;
        }
    }

public:
    dot_ast_t(const IIast_t *ast) : ast_(ast) { add_node(ast_->root()); }

    const std::unordered_map<int, const ast_node_t *> &nodes() const
    {
        return nodes_;
    }
    const std::vector<dot_edge_t> &edges() const { return edges_; }
};

class ast_dumper final {
    std::ostream *debug_stream_;
    ast_node_dumper node_dumper_;

private:
    template <typename Vec> void dump_edges_invis(const Vec &edges) const
    {
        for (auto &&it : edges)
            *debug_stream_ << "\t" << it.line.first << " -> " << it.line.second
                           << " [style=invis]\n";
    }

    template <typename Map> void dump_nodes(const Map &nodes) const
    {
        for (auto &&it : nodes)
            node_dumper_(*(it.second), it.first);
    }

    template <typename Vec> void dump_edges(const Vec &edges) const
    {
        for (auto &&it : edges)
            *debug_stream_ << "\t" << it.line.first << " -> " << it.line.second
                           << " [style=solid label=\"" << it.label << "\"]\n";
    }

public:
    ast_dumper(std::ostream *ds = &std::cout)
        : debug_stream_(ds), node_dumper_(ds)
    {}

    void operator()(const IIast_t &ast) const
    {
        dot_ast_t dot_ast(&ast);
        *debug_stream_ << "digraph \"AST\"\n{\n";
        dump_edges_invis(dot_ast.edges());
        dump_nodes(dot_ast.nodes());
        dump_edges(dot_ast.edges());
        *debug_stream_ << "}";
    }
};
} // namespace AST