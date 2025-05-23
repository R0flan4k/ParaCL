#pragma once

#include "AST.h"
#include "ast_representation.h"
#include "driver_exceptions.h"
#include "lexer.h"
#include "parser.tab.hh"
#include "symbol_table.h"

#include <string>
#include <string_view>

namespace yy {

class DriverPCL final {
    LexerPCL *plex_;
    std::ostream *report_stream_;
    std::string file_name_;

public:
    DriverPCL(LexerPCL *plex, std::string_view fn,
              std::ostream *rs = &std::cerr)
        : plex_(plex), report_stream_(rs), file_name_(fn)
    {}

    parser::token_type yylex(parser::semantic_type *yylval, location_t *loc)
    {
        parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
        *loc = plex_->get_loc();
        if (tt == yy::parser::token_type::NUMBER)
            yylval->as<int>() = std::stoi(plex_->YYText());
        if (tt == yy::parser::token_type::IDENT)
        {
            yy::parser::semantic_type tmp;
            tmp.as<std::string>() = plex_->YYText();
            yylval->move<std::string>(tmp);
        }
        if (tt == yy::parser::token_type::ERROR)
        {
            report_error("Unrecognized lexem " + std::string{plex_->YYText()},
                         *loc);
            throw ExceptsPCL::compilation_error("");
        }
        return tt;
    }

    void report_error(std::string_view report_str, const location_t &loc) const
    {
        auto err_line_str = plex_->get_str(loc.first_line);
        std::size_t ntabs =
            std::count(err_line_str.cbegin(), err_line_str.cend(), '\t');
        auto ptr_str = "\t  " + std::string(ntabs, '\t') +
                       std::string(loc.first_column - ntabs, ' ') + '^';
        *report_stream_ << file_name_ << ':' << loc.first_line + 1 << ':'
                        << loc.first_column + 1 << ": Error: " << report_str
                        << '.' << std::endl
                        << "   " << loc.first_line + 1 << "\t| " << err_line_str
                        << std::endl
                        << ptr_str << std::endl;
    }

    bool parse(AST::ast_representation_t *astr)
    {
        parser parser(this, astr);
        // parser.set_debug_level(true);
        return !parser.parse();
    }
};

} // namespace yy