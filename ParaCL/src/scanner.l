%option c++ noyywrap
%option yyclass="yy::LexerPCL"

%{
#include "parser.tab.hh"
#include "lexer.h"
%}

WS     [ \t\v]+
DIGIT  [0-9]
DIGIT1 [1-9]
LETTER [a-zA-Z_]
ID     {LETTER}({LETTER}|{DIGIT})*
NUM    {DIGIT}+

%%

{WS}
[\n]    { inc_str_i(); }
"if"    return yy::parser::token_type::IF;
"else"  return yy::parser::token_type::ELSE;
"while" return yy::parser::token_type::WHILE;
"print" return yy::parser::token_type::PRINT;
"?"     return yy::parser::token_type::WRITE;
"+"     return yy::parser::token_type::PLUS;
"-"     return yy::parser::token_type::MINUS;
"*"     return yy::parser::token_type::MULTIPLICATION;
"/"     return yy::parser::token_type::DIVISION;
"="     return yy::parser::token_type::ASSIGNMENT;
">"     return yy::parser::token_type::GREATER;
"<"     return yy::parser::token_type::LESS;
">="    return yy::parser::token_type::GREATEREQ;
"<="    return yy::parser::token_type::LESSEQ;
"=="    return yy::parser::token_type::EQUAL;
"!="    return yy::parser::token_type::NOTEQUAL;
";"     return yy::parser::token_type::SEMICOLON;
"("     return yy::parser::token_type::LPAR;
")"     return yy::parser::token_type::RPAR;
"{"     return yy::parser::token_type::LCURLY;
"}"     return yy::parser::token_type::RCURLY;
"&&"    return yy::parser::token_type::LAND;
"||"    return yy::parser::token_type::LOR;
"!"     return yy::parser::token_type::LNO;
"%"     return yy::parser::token_type::MODDIV; 
{NUM}   return yy::parser::token_type::NUMBER;
{ID}    return yy::parser::token_type::IDENT; 
.       return yy::parser::token_type::ERROR;

%%