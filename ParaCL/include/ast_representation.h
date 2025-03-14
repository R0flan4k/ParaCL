#pragma once

#include "AST.h"
#include "AST_dumper.h"
#include "symbol_table.h"

namespace yy {

class ast_representation_t final {
    AST::ast_t ast_;
    AST::symbol_table_t st_;

public:
    using node_ptr = typename AST::ast_t::node_ptr;
    using iterator = typename AST::symbol_table_t::iterator;

    ast_representation_t() : ast_(), st_() {}
    const AST::symbol_table_t &get_st() const { return st_; }
    const AST::IIast_t &get_ast() const { return ast_; }

    void set_root(node_ptr root) { return ast_.set_root(root); }

    iterator add_name(std::string name) { return st_.add_name(name); }

    bool is_in_symbol_table(const std::string &name) const
    {
        return st_.find(name) != st_.cend();
    }

    void execute() const { ast_.execute(st_); }
};

class astr_dumper final {
    AST::ast_dumper ast_dumper_;
    AST::symbol_table_dumper st_dumper_;

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

} // namespace yy