%language "c++"

%locations
%skeleton "lalr1.cc"
%define api.value.type variant
%param {yy::DriverPCL* driver}
%parse-param {yy::ast_representation_t* astr}
%expect 53

/* Generate the parser description file. */
 %verbose
/* Enable run-time traces (yydebug). */
 %define parse.trace 

%define parse.error verbose
%define api.location.type {location_t}

%code requires 
{
    #include "AST.h"
    #include "symbol_table.h"
    #include "driver_exceptions.h"
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
    parser::token_type yylex(parser::semantic_type *yylval, location_t* loc,
                             DriverPCL *driver);
    }

    #define YYLLOC_DEFAULT(Cur, Rhs, N)                   \
    do                                                    \
    if (N)                                                \
    {                                                     \
      (Cur) = YYRHSLOC(Rhs, N);                           \
    }                                                     \
    else                                                  \
    {                                                     \
      (Cur).first_line   = (Cur).last_line   =            \
        YYRHSLOC(Rhs, 0).last_line;                       \
      (Cur).first_column = (Cur).last_column =            \
        YYRHSLOC(Rhs, 0).last_column;                     \
    }                                                     \
    while (0)

    std::ostream& operator<<(std::ostream& stream, const location_t& loc) 
    {
        return stream << loc.first_line << " " << loc.first_column << " " << loc.last_line << " " << loc.last_column;
    }
}

%token
    IF              "if"
    ELSE            "else"
    WHILE           "while"
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
    LCURLY          "{"
    RCURLY          "}"
    LPAR            "("
    RPAR            ")"
    LAND            "&&"
    LOR             "||"
    LNO             "!"
    MODDIV          "%"
    ERROR
;

%token <number_tt> NUMBER
%token <ident_tt> IDENT

%nterm <stmts_nt>     stmts
%nterm <expr_nt>      decl
%nterm <expr_nt>      expr
%nterm <lval_nt>      lval
%nterm <expr_nt>      epn
%nterm <assign_op_nt> apn
%nterm <expr_nt>      tpn
%nterm <expr_nt>      fn
%nterm <node_nt>      cndtl
%nterm <ifst_nt>      ifst
%nterm <ifst_nt>  ifelsest
%nterm <whilest_nt>   whilest
%nterm <expr_nt>      cond
%nterm <node_nt>      body
%nterm <expr_nt>      logics

%start program

%%
program: stmts              { astr->set_root($1); }
;

stmts: expr SEMICOLON stmts { $$ = make_node<stmts_nt>($1, $3); }
     | cndtl stmts          { $$ = make_node<stmts_nt>($1, $2); }
     | SEMICOLON stmts      { $$ = std::move($2); }
     | %empty               { $$ = make_node<stmts_nt>(); }
;

expr: decl                  { $$ = std::move($1); }
    | logics                { $$ = std::move($1); }
;

cndtl: ifelsest             { $$ = std::move($1); }
     | whilest              { $$ = std::move($1); }
;

ifelsest: ifst ELSE ifelsest  { $$ = make_node<ifelsest_nt>($1, $3); }
        | ifst ELSE body      { $$ = make_node<ifelsest_nt>($1, $3); }
        | ifst                { $$ = std::move($1); }
;

ifst: IF cond body          { $$ = make_node<ifst_nt>($2, $3); }
;

whilest: WHILE cond body    { $$ = make_node<whilest_nt>($2, $3); }
;

cond: LPAR expr RPAR        { $$ = std::move($2); }
;

body: LCURLY stmts RCURLY   { $$ = std::move($2); }
    | SEMICOLON             { $$ = make_node<empty_op_nt>(); }
    | expr SEMICOLON        { $$ = std::move($1); }
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

logics: logics LAND epn     { $$ = make_node<logical_and_op_nt>($1, $3); }
      | logics LOR  epn     { $$ = make_node<logical_or_op_nt>($1, $3); }
      | epn
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
   | tpn MODDIV         fn  { $$ = make_node<mod_div_op_nt>($1, $3); }
   | fn
;

fn: LPAR expr RPAR          { $$ = std::move($2); }
  | NUMBER                  { $$ = make_node<number_nt>($1); }
  | IDENT                   { 
                              if (!(astr->is_in_symbol_table($1))) 
                                throw yy::parser::syntax_error
                                  (@$, "Undefined variable: " + std::string($1));
                              $$ = make_node<var_nt>($1); 
                            }
  | WRITE                   { $$ = make_node<write_nt>(); }
  | PRINT expr              { $$ = make_node<print_op_nt>($2); }
  | MINUS fn                { $$ = make_node<unminus_op_nt>($2); }
  | PLUS  fn                { $$ = make_node<unplus_op_nt>($2); }
  | LNO   fn                { $$ = make_node<logical_no_op_nt>($2); }
;

%%

namespace yy {
    void parser::error(const location_t& loc, const std::string& str) {
        driver->report_error(str, loc);
        throw ExceptsPCL::compilation_error("");
    }

    parser::token_type yylex(parser::semantic_type *yylval,
                             location_t *loc,
                             DriverPCL *driver)
    {
        return driver->yylex(yylval, loc);
    }
}
