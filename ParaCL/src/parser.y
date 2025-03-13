%language "c++"

%skeleton "lalr1.cc"
%define api.value.type variant
%param {yy::DriverPCL* driver}
%parse-param {yy::ast_representation_t* astr}

/* Generate the parser description file. */
 %verbose
/* Enable run-time traces (yydebug). */
 %define parse.trace 

%code requires 
{
    #include "AST.h"
    #include "symbol_table.h"
    #include <memory>
    #include <string>

    namespace yy {
    class DriverPCL;
    class ast_representation_t;
    }

    #include "parser.h"
}

%code
{
    #include "paracl.h"

    namespace yy {
    parser::token_type yylex(parser::semantic_type *yylval,
                             DriverPCL *driver);
    }
}

%token
    PRINT           "print"
    WRITE           "?"
    PLUS            "+"
    MINUS           "-"
    MULTIPLICATION  "*"   
    DIVISION        "/"
    ASSIGNMENT      "="
    GREATER         ">"
    LESS            "<"
    GREATEREQ       ">="
    LESSEQ          "<="
    EQUAL           "=="
    NOTEQUAL        "!="
    SEMICOLON       ";"
    LPAR            "("
    RPAR            ")"
    LCURLY          "{"
    RCURLY          "}"
    ERROR
;

%token <number_tt> NUMBER
%token <ident_tt> IDENT

%nterm <stmts_nt>   stmts
%nterm <expr_nt>    decl
%nterm <expr_nt>    expr
%nterm <lval_nt>     lval
%nterm <expr_nt>    epn
%nterm <assign_op_nt>    apn
%nterm <expr_nt>    tpn
%nterm <expr_nt>    fn

%start program

%%
program: stmts              { astr->set_root($1); }
;

stmts: expr SEMICOLON stmts { $$ = make_node<stmts_nt>($1, $3); }
     | SEMICOLON stmts      { $$ = std::move($2); }
     | %empty               { }
;

expr: decl                  { $$ = std::move($1); }
    | epn                   { $$ = std::move($1); }
;

decl: lval apn              { 
                              $$ = make_node<assign_op_nt>($1, $2);
                              astr->add_name($1->name);
                            }
;

lval: IDENT                 { 
                              $$ = make_node<lval_nt>($1);
                            }
;

apn: ASSIGNMENT expr        { $$ = make_node<assign_op_nt>($2); }
;

epn: epn PLUS      tpn      { $$ = make_node<plus_op_nt>($1, $3); }
   | epn MINUS     tpn      { $$ = make_node<minus_op_nt>($1, $3); }
   | epn EQUAL     tpn      { $$ = make_node<equal_op_nt>($1, $3); }
   | epn NOTEQUAL  tpn      { $$ = make_node<notequal_op_nt>($1, $3); }
   | epn GREATER   tpn      { $$ = make_node<greater_op_nt>($1, $3); }
   | epn LESS      tpn      { $$ = make_node<less_op_nt>($1, $3); }
   | epn GREATEREQ tpn      { $$ = make_node<greatereq_op_nt>($1, $3); }
   | epn LESSEQ    tpn      { $$ = make_node<lesseq_op_nt>($1, $3); }
   | tpn                    { $$ = std::move($1); }
;

tpn: tpn MULTIPLICATION fn  { $$ = make_node<mul_op_nt>($1, $3); }
   | tpn DIVISION       fn  { $$ = make_node<div_op_nt>($1, $3); }
   | fn
;

fn: LPAR expr RPAR          { $$ = std::move($2); }
  | NUMBER                  { $$ = make_node<number_nt>($1); }
  | IDENT                   { 
                              if (!(astr->is_in_symbol_table($1))) 
                                throw yy::parser::syntax_error
                                  ("Undefined variable: " + std::string($1));
                              $$ = make_node<var_nt>($1); 
                            }
  | WRITE                   { $$ = make_node<write_nt>(); }
  | PRINT expr              { $$ = make_node<print_op_nt>($2); }
;

%%

namespace yy {
    void parser::error(const std::string& str) {
        std::cout << str;
        std::terminate();
    }

    parser::token_type yylex(parser::semantic_type *yylval,
                             DriverPCL *driver)
    {
        return driver->yylex(yylval);
    }
}