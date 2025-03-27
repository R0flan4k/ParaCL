#pragma once

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include "parser.h"

#include <iostream>
#include <sstream>
#include <string>

namespace yy {

class LexerPCL final : public yyFlexLexer {
    location_t loc_{};
    std::string text;

    using StrIt = typename std::string::const_iterator;
    std::vector<StrIt> strs_;
    StrIt pos_;

public:
    LexerPCL(std::stringstream *istream)
        : yyFlexLexer(istream), text(istream->str()), pos_(text.cbegin())
    {
        update_line();
    }

    std::string get_str(int i) const
    {
        auto start = strs_[i], end = std::find(start, text.cend(), '\n');
        return std::string{start, end};
    }
    const location_t &get_loc() const { return loc_; }
    int yylex() override;

private:
    void update_line() { strs_.emplace_back(pos_); }

    int update_location()
    {
        loc_.first_line = loc_.last_line;
        loc_.first_column = loc_.last_column;
        int i = 0;
        for (; yytext[i] != '\0'; i++)
        {
            if (yytext[i] == '\n')
            {
                loc_.last_line++;
                loc_.last_column = 0;
            }
            else
            {
                loc_.last_column++;
            }
        }
        return i;
    }
};

} // namespace yy