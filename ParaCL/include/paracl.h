#pragma once

#include "AST.h"
#include "ast_representation.h"
#include "parser.tab.hh"
#include "symbol_table.h"

#include <FlexLexer.h>

#include <string>

namespace yy {

class DriverPCL final {
    FlexLexer *plex_;

public:
    DriverPCL(FlexLexer *plex) : plex_(plex) {}

    parser::token_type yylex(parser::semantic_type *yylval)
    {
        parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
        if (tt == yy::parser::token_type::NUMBER)
            yylval->as<int>() = std::stoi(plex_->YYText());
        if (tt == yy::parser::token_type::IDENT)
        {
            yy::parser::semantic_type tmp;
            tmp.as<std::string>() = plex_->YYText();
            yylval->move<std::string>(tmp);
        }
        if (tt == yy::parser::token_type::ERROR)
            std::cerr << "Unrecognized lexem." << std::endl;
        return tt;
    }

    bool parse(ast_representation_t *astr)
    {
        parser parser(this, astr);
        // parser.set_debug_level(true);
        return !parser.parse();
    }
};

} // namespace yy