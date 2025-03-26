#pragma once

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include <iostream>
#include <sstream>

namespace yy {

class LexerPCL final : public yyFlexLexer {
    int str_i = 1;
    // std::vector<>

public:
    LexerPCL(std::stringstream *istream) : yyFlexLexer(istream) {}

    int cur_str_id() const { return str_i; }
    int yylex() override;

private:
    void inc_str_i()
    {
        ++str_i;
#if 0
        char c = yyin.peek();
#endif
    }
};

} // namespace yy