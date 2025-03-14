#pragma once

#include "AST.h"

#include <iostream>

namespace AST {

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
        case node_types::IF:
            return "if";
            break;
        case node_types::IFELSE:
            return "if else";
            break;
        case node_types::WHILE:
            return "while";
            break;
        case node_types::EMPTY:
            return "empty";
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
        case node_types::IF:
        {
            int cond_id = ids++, body_id = ids++;
            nodes_.try_emplace(id, &node);
            edges_.push_back({id, cond_id, "cond"});
            edges_.push_back({id, body_id, "body"});
            add_node(*(static_cast<const ast_if_t &>(node).condition), cond_id);
            add_node(*(static_cast<const ast_if_t &>(node).body), body_id);
            break;
        }
        case node_types::IFELSE:
        {
            int cond_id = ids++, body_id = ids++, else_body_id = ids++;
            nodes_.try_emplace(id, &node);
            edges_.push_back({id, cond_id, "cond"});
            edges_.push_back({id, body_id, "body"});
            edges_.push_back({id, else_body_id, "else_body"});
            add_node(*(static_cast<const ast_ifelse_t &>(node).condition),
                     cond_id);
            add_node(*(static_cast<const ast_ifelse_t &>(node).body), body_id);
            add_node(*(static_cast<const ast_ifelse_t &>(node).else_body),
                     else_body_id);
            break;
        }
        case node_types::WHILE:
        {
            int cond_id = ids++, body_id = ids++;
            nodes_.try_emplace(id, &node);
            edges_.push_back({id, cond_id, "cond"});
            edges_.push_back({id, body_id, "body"});
            add_node(*(static_cast<const ast_while_t &>(node).condition),
                     cond_id);
            add_node(*(static_cast<const ast_while_t &>(node).body), body_id);
            break;
        }
        case node_types::WRITE:
        case node_types::LVAL:
        case node_types::NUMBER:
        case node_types::VARIABLE:
        case node_types::EMPTY:
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