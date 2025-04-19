#pragma once

#include "AST.h"

#include <memory>
#include <string>

using number_tt = int;
using ident_tt = std::string;
using nterm_nt = AST::ast_node_t::node_it;

using AST::ast_bin_op_t;
using AST::ast_empty_op_t;
using AST::ast_expr_t;
using AST::ast_if_t;
using AST::ast_ifelse_t;
using AST::ast_lval_t;
using AST::ast_node_t;
using AST::ast_num_t;
using AST::ast_scope_t;
using AST::ast_statements_t;
using AST::ast_un_op_t;
using AST::ast_var_t;
using AST::ast_while_t;
using AST::ast_write_t;

using AST::ast_assign_op;
using AST::ast_div_op;
using AST::ast_equal_op;
using AST::ast_greater_op;
using AST::ast_greatereq_op;
using AST::ast_less_op;
using AST::ast_lesseq_op;
using AST::ast_logical_and_op;
using AST::ast_logical_no_op;
using AST::ast_logical_or_op;
using AST::ast_minus_op;
using AST::ast_modular_division_op;
using AST::ast_mul_op;
using AST::ast_notequal_op;
using AST::ast_plus_op;
using AST::ast_print_op;
using AST::ast_unminus_op;
using AST::ast_unplus_op;

using bin_ops = AST::ast_bin_ops;
using un_ops = AST::ast_un_ops;

struct location_t {
    std::size_t first_line, first_column, last_line, last_column;
};