#pragma once

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include <iostream>

namespace yy {

class LexerPCL final : public yyFlexLexer {
    int str_i = 1;

public:
    LexerPCL(std::istream *istream) : yyFlexLexer(istream) {}

    int cur_str() const { return str_i; }
    int yylex() override;

private:
    void inc_str_i() { ++str_i; }
};

} // namespace yy