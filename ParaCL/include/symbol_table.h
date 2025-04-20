#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>

namespace AST {

template <typename T, typename U> using BaseMap = std::unordered_map<T, U>;
using map_it = BaseMap<std::string_view, int>::iterator;
using scope_t = std::vector<map_it>;

class scopes_t final : private std::stack<scope_t> {
public:
    void add_name(map_it it)
    {
        assert(!empty());
        top().push_back(it);
    }

    using std::stack<scope_t>::empty;
    using std::stack<scope_t>::size;
    using std::stack<scope_t>::top;
    using std::stack<scope_t>::emplace;
    using std::stack<scope_t>::pop;
};

class symbol_table_t final : private BaseMap<std::string_view, int> {
    scopes_t scopes_;

public:
    using iterator = map_it;
    using const_iterator =
        typename BaseMap<std::string_view, int>::const_iterator;

    symbol_table_t() {}

    using BaseMap<std::string_view, int>::begin;
    using BaseMap<std::string_view, int>::end;
    using BaseMap<std::string_view, int>::cbegin;
    using BaseMap<std::string_view, int>::cend;
    using BaseMap<std::string_view, int>::empty;
    using BaseMap<std::string_view, int>::size;
    using BaseMap<std::string_view, int>::find;

    iterator add_name(std::string_view name)
    {
        auto insertion = insert({name, 0});
        if (insertion.second)
            scopes_.add_name(insertion.first);
        return insertion.first;
    }

    void emplace_scope() { scopes_.emplace(); }

    void pop_scope()
    {
        auto &cur_scope = scopes_.top();
        for (auto &&pos : cur_scope)
            erase(pos);
        scopes_.pop();
    }
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
        for (auto &&p : st)
            *debug_stream_ << "\t" << p.first << std::endl;
    }
};

} // namespace AST