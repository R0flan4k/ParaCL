#pragma once

#include <list>
#include <string_view>
#include <unordered_map>

#include "AST.h"
#include "AST_dumper.h"
#include "symbol_table.h"

namespace AST {

class ast_representation_t final {
    ast_t ast_;
    symbol_table_t st_;

public:
    using node_it = typename ast_t::node_it;
    using iterator = typename symbol_table_t::iterator;

    ast_representation_t() : ast_() {}
    const symbol_table_t &get_st() const { return st_; }
    const IIast_t &get_ast() const { return ast_; }

    void set_root(node_it root) { return ast_.set_root(root); }

    template <typename T, typename... Args> node_it make_node(Args &&... args)
    {
        return ast_.make_node<T, Args...>(std::forward<Args>(args)...);
    }
    template <typename T, typename... Args>
    node_it make_node_st(Args &&... args)
    {
        return ast_.make_node<T, Args...>(std::forward<Args>(args)..., st_);
    }

    void pop_scope() { st_.pop_scope(); }
    void emplace_scope() { st_.emplace_scope(); }
    iterator add_name(std::string_view name) { return st_.add_name(name); }
    bool is_in_symbol_table(std::string_view name) const
    {
        return st_.find(std::string{name}) != st_.cend();
    }

    void execute() { ast_.execute(st_); }
};

class astr_dumper final {
    ast_dumper ast_dumper_;
    symbol_table_dumper st_dumper_;

public:
    astr_dumper(std::ostream *ast_stream, std::ostream *st_stream = &std::cout)
        : ast_dumper_(ast_stream), st_dumper_(st_stream)
    {}

    void operator()(const ast_representation_t &astr) const
    {
        ast_dumper_(astr.get_ast());
        st_dumper_(astr.get_st());
    }
};

} // namespace AST