#pragma once

#include "AST.h"

#include <memory>
#include <string>

using number_tt = int;
using ident_tt = std::string;

using number_nt = typename std::shared_ptr<AST::ast_num_t>;
using var_nt = typename std::shared_ptr<AST::ast_var_t>;
using lval_nt = typename std::shared_ptr<AST::ast_lval_t>;
using stmts_nt = typename std::shared_ptr<AST::ast_statements_t>;
using stmt_nt = typename std::shared_ptr<AST::ast_expr_t>;
using expr_nt = typename std::shared_ptr<AST::ast_expr_t>;
using write_nt = typename std::shared_ptr<AST::ast_write_t>;
using bin_op_nt = typename std::shared_ptr<AST::ast_bin_op_t>;
using un_op_nt = typename std::shared_ptr<AST::ast_un_op_t>;
using node_nt = typename std::shared_ptr<AST::ast_node_t>;
using ifst_nt = typename std::shared_ptr<AST::ast_if_t>;
using ifelsest_nt = typename std::shared_ptr<AST::ast_ifelse_t>;
using whilest_nt = typename std::shared_ptr<AST::ast_while_t>;
using empty_op_nt = typename std::shared_ptr<AST::ast_empty_op_t>;

using assign_op_nt = typename std::shared_ptr<AST::ast_assign_op>;
using div_op_nt = typename std::shared_ptr<AST::ast_div_op>;
using mul_op_nt = typename std::shared_ptr<AST::ast_mul_op>;
using plus_op_nt = typename std::shared_ptr<AST::ast_plus_op>;
using minus_op_nt = typename std::shared_ptr<AST::ast_minus_op>;
using greater_op_nt = typename std::shared_ptr<AST::ast_greater_op>;
using less_op_nt = typename std::shared_ptr<AST::ast_less_op>;
using greatereq_op_nt = typename std::shared_ptr<AST::ast_greatereq_op>;
using lesseq_op_nt = typename std::shared_ptr<AST::ast_lesseq_op>;
using equal_op_nt = typename std::shared_ptr<AST::ast_equal_op>;
using notequal_op_nt = typename std::shared_ptr<AST::ast_notequal_op>;
using print_op_nt = typename std::shared_ptr<AST::ast_print_op>;
using unminus_op_nt = typename std::shared_ptr<AST::ast_unminus_op>;
using unplus_op_nt = typename std::shared_ptr<AST::ast_unplus_op>;

using AST::make_node;
using AST::node_cast;

using bin_ops = AST::ast_bin_ops;
using un_ops = AST::ast_un_ops;