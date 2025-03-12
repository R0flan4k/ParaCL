%language "c++"

%skeleton "lalr1.cc"
%define api.value.type variant
%param {yy::DriverPCL* driver}
%parse-param {yy::ast_representation_t* astr}


/* Generate the parser description file. */
 %verbose
/* Enable run-time traces (yydebug). */
 %define parse.trace 

/* Formatting semantic values. */
/* %printer { fprintf (yyo, "%s", $$->name); } VAR;   */
/* %printer { fprintf (yyo, "%s()", $$->name); } FUN; */
/* %printer { fprintf (yyo, "%d", $$); } <int>;       */


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

    #define YYDEBUG 1
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
    EQUAL           "=="
    NOTEQUAL        "!="
    SEMICOLON       ";"
    LPAR            "("
    RPAR            ")"
    ERROR
;

%token <number_tt> NUMBER
%token <ident_tt> IDENT

%nterm <exprs_nt>   exprs
%nterm <expr_nt>    expr
%nterm <lval_nt>     lval
%nterm <expr_nt>    en
%nterm <expr_nt>    epn
%nterm <assign_op_nt>    apn
%nterm <expr_nt>    tn
%nterm <expr_nt>    tpn
%nterm <expr_nt>    fn
%nterm <expr_nt>  decl

%start program

%%
program: exprs              { astr->set_root($1); }
;

exprs: decl SEMICOLON exprs { $$ = make_node<exprs_nt>($1, $3); }
     | expr SEMICOLON exprs { $$ = make_node<exprs_nt>($1, $3); }
     | %empty               { $$ = make_node<exprs_nt>(); }
;

decl: lval apn              { 
                              $$ = make_node<assign_op_nt>($1, $2);
                              /* node_cast<assign_op_nt>($2)->lhs = std::move($1);
                              $$ = std::move($2); 
                              node_cast<assign_op_nt>($$)->lhs = std::move($1);  <--- VOT SDES. SDELAYU CONSTRUCTOR NOVIY  */
                              astr->add_name($1->name);
                            }
;

expr: en                    { $$ = std::move($1); }
;

lval: IDENT                 { 
                              $$ = make_node<lval_nt>($1);
                            }
;

apn: ASSIGNMENT expr        { $$ = make_node<assign_op_nt>($2); }
;

en: tn                      { $$ = std::move($1); }
  | tn epn                  {
                              $$ = std::move($2);
                              node_cast<bin_op_nt>($$)->lhs = std::move($1);
                            }
;

epn: PLUS     en            { $$ = make_node<plus_op_nt>($2); }
   | MINUS    en            { $$ = make_node<minus_op_nt>($2); }
   | EQUAL    en            { $$ = make_node<equal_op_nt>($2); }
   | NOTEQUAL en            { $$ = make_node<notequal_op_nt>($2); }
   | GREATER  en            { $$ = make_node<greater_op_nt>($2); }
   | LESS     en            { $$ = make_node<less_op_nt>($2); }
;

tn: fn                      { $$ = std::move($1); }
  | fn tpn                  {
                              $$ = std::move($2);
                              node_cast<bin_op_nt>($$)->lhs = std::move($1);
                            }
;

tpn: MULTIPLICATION tn      { $$ = make_node<mul_op_nt>($2); }
   | DIVISION       tn      { $$ = make_node<div_op_nt>($2); }
;

fn: LPAR en RPAR            { $$ = std::move($2); }
  | NUMBER                  { $$ = make_node<number_nt>($1); }
  | IDENT                   { 
                              if (!(astr->is_in_symbol_table($1))) 
                                throw yy::parser::syntax_error
                                  ("Undefined variable: " + std::string($1));
                              $$ = make_node<var_nt>($1); 
                            }
  | WRITE                   { $$ = make_node<write_nt>(); }
  | PRINT en                { $$ = make_node<print_op_nt>($2); }
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