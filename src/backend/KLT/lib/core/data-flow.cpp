
#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/kernel.hpp"

#include "sage3basic.h"

#include <cassert>

namespace KLT {

namespace Core {

void append_access(
  SgExpression * exp,
  std::set<Data *> & access_set,
  const std::set<Data *> & datas
) {
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(exp);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();
    assert(var_sym != NULL);

    Data * data = NULL;
    std::set<Data *>::const_iterator it_data;
    for (it_data = datas.begin(); it_data != datas.end(); it_data++)
      if ((*it_data)->getVariableSymbol() == var_sym) {
        data = *it_data;
        break;
      }
    if (data == NULL)
      continue;

    access_set.insert(data);
  }
}

void compute_read_write(
  LoopTrees::node_t * tree,
  Kernel::dataflow_t & data_flow,
  const std::set<Data *> & datas
) {
  std::set<SgStatement *> leaves;
  collectLeaves(tree, leaves);

  std::set<SgStatement *>::iterator it_stmt;
  for (it_stmt = leaves.begin(); it_stmt != leaves.end(); it_stmt++) {
    SgExprStatement * expr_stmt = isSgExprStatement(*it_stmt);
    if (expr_stmt != NULL) {
      SgExpression * exp = expr_stmt->get_expression();
      assert(exp != NULL);

      SgBinaryOp * bin_op = isSgBinaryOp(exp);
      if (bin_op != NULL) {
        SgExpression * lhs_exp = bin_op->get_lhs_operand_i();
        SgExpression * rhs_exp = bin_op->get_rhs_operand_i();

        assert(lhs_exp != NULL && rhs_exp != NULL);

        SgAssignOp * assign_op = isSgAssignOp(exp);
        SgCompoundAssignOp * compound_assign_op = isSgCompoundAssignOp(exp);

        assert((assign_op != NULL) xor (compound_assign_op != NULL)); // FIXME expression statement are not always made of assignement

        if (assign_op != NULL || compound_assign_op != NULL) {
          append_access(lhs_exp, data_flow.write, datas); // add access in lhs to write set
          append_access(rhs_exp, data_flow.read,  datas); // add access in rhs to read set
        }
        if (compound_assign_op != NULL) {
          append_access(lhs_exp, data_flow.read,  datas); // add access in lhs to read set
        }
      }
      else assert(false); // FIXME expression statement are not always made of binary op
    }
    else assert(false); // FIXME case other than expression statement (replacing the set of leave by a FIFO)
  }

  Data::set_union(data_flow.datas, data_flow.read, data_flow.write);
}

}

}

