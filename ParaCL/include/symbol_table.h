#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace AST {

enum class syms {
    VARIABLE,
};

using el_val_t = int;

struct symbol_table_el_t {
    syms type;
    std::string name;

    symbol_table_el_t(syms typee, const std::string &namee)
        : type(typee), name(namee)
    {}
    virtual el_val_t get() const = 0;
    virtual void set(const el_val_t &) = 0;
    virtual std::shared_ptr<symbol_table_el_t> copy() const = 0;
    virtual ~symbol_table_el_t() = default;
};

struct symbol_table_var_t final : public symbol_table_el_t {
    el_val_t val;

    symbol_table_var_t(const std::string &name)
        : symbol_table_el_t(syms::VARIABLE, name)
    {}
    el_val_t get() const override { return val; }
    void set(const el_val_t &v) override { val = v; }
    std::shared_ptr<symbol_table_el_t> copy() const override
    {
        return std::make_unique<symbol_table_var_t>(*this);
    }
};

struct var_val_t final {
    int val;
    var_val_t() {}
    void set(int vall) { val = vall; }
    int get() const { return val; }
};

class symbol_table_t final
    : private std::unordered_map<std::string, var_val_t> {
    using ElT = var_val_t;

public:
    using iterator = typename std::unordered_map<std::string, ElT>::iterator;
    using const_iterator =
        typename std::unordered_map<std::string, ElT>::const_iterator;

    symbol_table_t() {}

    using std::unordered_map<std::string, var_val_t>::begin;
    using std::unordered_map<std::string, var_val_t>::end;
    using std::unordered_map<std::string, var_val_t>::cbegin;
    using std::unordered_map<std::string, var_val_t>::cend;
    using std::unordered_map<std::string, var_val_t>::empty;
    using std::unordered_map<std::string, var_val_t>::size;
    using std::unordered_map<std::string, var_val_t>::find;

    iterator add_name(std::string name) { return insert({name, ElT()}).first; }
};

class symbol_table_dumper final {
    std::ostream *debug_stream_;

public:
    symbol_table_dumper(std::ostream *ds) : debug_stream_(ds) {}

    void operator()(const symbol_table_t &st) const
    {
        *debug_stream_ << "Symbol table dump:" << std::endl;
        if (st.empty())
        {
            *debug_stream_ << "[EMPTY]" << std::endl;
            return;
        }
        *debug_stream_ << "(Size) " << st.size() << std::endl
                       << "(Names)" << std::endl;
        for (auto start = st.cbegin(), end = st.cend(); start != end; ++start)
            *debug_stream_ << "\t" << start->first << std::endl;
    }
};

} // namespace AST