%option c++ noyywrap

%{
#include "parser.tab.hh"
%}

WS     [ \t\n\v]+
DIGIT  [0-9]
DIGIT1 [1-9]
LETTER [a-zA-Z_]
ID     {LETTER}({LETTER}|{DIGIT})*
NUM    {DIGIT}+

%%

{WS}
"print" return yy::parser::token_type::PRINT;
"?"     return yy::parser::token_type::WRITE;
"+"     return yy::parser::token_type::PLUS;
"-"     return yy::parser::token_type::MINUS;
"*"     return yy::parser::token_type::MULTIPLICATION;
"/"     return yy::parser::token_type::DIVISION;
"="     return yy::parser::token_type::ASSIGNMENT;
">"     return yy::parser::token_type::GREATER;
"<"     return yy::parser::token_type::LESS;
"=="    return yy::parser::token_type::EQUAL;
"!="    return yy::parser::token_type::NOTEQUAL;
";"     return yy::parser::token_type::SEMICOLON;
"("     return yy::parser::token_type::LPAR;
")"     return yy::parser::token_type::RPAR;
{NUM}   return yy::parser::token_type::NUMBER;
{ID}    { return yy::parser::token_type::IDENT; std::cout << "ID\n"; } 
.       return yy::parser::token_type::ERROR;

%%