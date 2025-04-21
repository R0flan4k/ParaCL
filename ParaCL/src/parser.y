%language "c++"

%locations
%skeleton "lalr1.cc"
%define api.value.type variant
%param {yy::DriverPCL* driver}
%parse-param {AST::ast_representation_t* astr}

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
    }

    namespace AST {
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

%nterm <nterm_nt>     stmts
%nterm <nterm_nt>      stmt
%nterm <nterm_nt>     scope
%nterm <nterm_nt>      decl
%nterm <nterm_nt>      expr
%nterm <nterm_nt>      lval
%nterm <nterm_nt>      epn
%nterm <nterm_nt>      apn
%nterm <nterm_nt>      tpn
%nterm <nterm_nt>      fn
%nterm <nterm_nt>      cndtl
%nterm <nterm_nt>      ifst
%nterm <nterm_nt>  ifelsest
%nterm <nterm_nt>   whilest
%nterm <nterm_nt>      cond
%nterm <nterm_nt>      body
%nterm <nterm_nt>      logics
%nterm <nterm_nt>      comp

%nterm scope_entry
%nterm scope_exit

%start program

%%
program: scope_entry stmts scope_exit { astr->set_root(astr->make_node<ast_scope_t>($2)); }
;

scope: LCURLY scope_entry stmts scope_exit RCURLY { $$ = astr->make_node<ast_scope_t>($3); }
;

scope_entry: %empty                   { astr->emplace_scope(); }
;

scope_exit: %empty                    { astr->pop_scope(); }
;

stmts: stmt stmts           { $$ = astr->make_node<ast_statements_t>($1, $2); }
     | %empty               { $$ = astr->make_node<ast_statements_t>(); }

stmt: expr SEMICOLON { $$ = $1; }
    | cndtl          { $$ = $1; }
    | scope          { $$ = $1; }
    | SEMICOLON      { $$ = astr->make_node<ast_empty_op_t>(); }
;

expr: decl                  { $$ = $1; }
    | logics                { $$ = $1; }
;

cndtl: ifelsest             { $$ = $1; }
     | whilest              { $$ = $1; }
;

ifelsest: ifst ELSE ifelsest  { $$ = astr->make_node<ast_ifelse_t>($1, $3); }
        | ifst ELSE body      { $$ = astr->make_node<ast_ifelse_t>($1, $3); }
        | ifst                { $$ = $1; }
;

ifst: IF cond body          { $$ = astr->make_node<ast_if_t>($2, $3); }
;

whilest: WHILE cond body    { $$ = astr->make_node<ast_while_t>($2, $3); }
;

cond: LPAR expr RPAR        { $$ = $2; }
;

body: stmt                  { $$ = $1; }
;

decl: lval ASSIGNMENT expr  { $$ = astr->make_node<ast_assign_op>($1, $3); }
;

lval: IDENT                 { 
                              $$ = astr->make_node_st<ast_lval_t>($1);
                            }
;

apn: expr                    { $$ = astr->make_node<ast_assign_op>($1); }
;

logics: logics LAND comp     { $$ = astr->make_node<ast_logical_and_op>($1, $3); }
      | logics LOR  comp     { $$ = astr->make_node<ast_logical_or_op>($1, $3); }
      | comp
;

comp: comp EQUAL     epn      { $$ = astr->make_node<ast_equal_op>($1, $3); }
    | comp NOTEQUAL  epn      { $$ = astr->make_node<ast_notequal_op>($1, $3); }
    | comp GREATER   epn      { $$ = astr->make_node<ast_greater_op>($1, $3); }
    | comp LESS      epn      { $$ = astr->make_node<ast_less_op>($1, $3); }
    | comp GREATEREQ epn      { $$ = astr->make_node<ast_greatereq_op>($1, $3); }
    | comp LESSEQ    epn      { $$ = astr->make_node<ast_lesseq_op>($1, $3); }
    | epn                     { $$ = $1; }
;

epn: epn PLUS      tpn      { $$ = astr->make_node<ast_plus_op>($1, $3); }
   | epn MINUS     tpn      { $$ = astr->make_node<ast_minus_op>($1, $3); }
   | tpn                    { $$ = $1; }
;

tpn: tpn MULTIPLICATION fn  { $$ = astr->make_node<ast_mul_op>($1, $3); }
   | tpn DIVISION       fn  { $$ = astr->make_node<ast_div_op>($1, $3); }
   | tpn MODDIV         fn  { $$ = astr->make_node<ast_modular_division_op>($1, $3); }
   | fn
;

fn: LPAR expr RPAR          { $$ = $2; }
  | NUMBER                  { $$ = astr->make_node<ast_num_t>($1); }
  | IDENT                   { 
                              try {
                                $$ = astr->make_node_st<ast_var_t>($1); 
                              } catch (ExceptsPCL::compilation_error &ce)
                              {
                                throw yy::parser::syntax_error
                                  (@$, ce.what());
                              }
                            }
  | WRITE                   { $$ = astr->make_node<ast_write_t>(); }
  | PRINT expr              { $$ = astr->make_node<ast_print_op>($2); }
  | MINUS fn                { $$ = astr->make_node<ast_unminus_op>($2); }
  | PLUS  fn                { $$ = astr->make_node<ast_unplus_op>($2); }
  | LNO   fn                { $$ = astr->make_node<ast_logical_no_op>($2); }
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
