#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

namespace AST {

using map_it = std::unordered_map<std::string, int>::iterator;
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

class symbol_table_t final : private std::unordered_map<std::string, int> {
    scopes_t scopes_;

public:
    using iterator = map_it;
    using const_iterator =
        typename std::unordered_map<std::string, int>::const_iterator;

    symbol_table_t() {}

    using std::unordered_map<std::string, int>::begin;
    using std::unordered_map<std::string, int>::end;
    using std::unordered_map<std::string, int>::cbegin;
    using std::unordered_map<std::string, int>::cend;
    using std::unordered_map<std::string, int>::empty;
    using std::unordered_map<std::string, int>::size;
    using std::unordered_map<std::string, int>::find;

    iterator add_name(std::string name)
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
        std::for_each(cur_scope.begin(), cur_scope.end(),
                      [&](map_it pos) { erase(pos); });
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
        std::for_each(st.cbegin(), st.cend(),
                      [&](const std::pair<std::string, int> &p) {
                          *debug_stream_ << "\t" << p.first << std::endl;
                      });
    }
};

} // namespace AST