
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/kernel.hpp"

#include "sage3basic.h"

#include <cassert>

namespace KLT {

namespace Core {

void DataFlow::append_access(
  SgExpression * exp,
  std::set<Data *> & access_set,
  const std::set<Data *> & datas
) const {
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

void DataFlow::compute_read_write(
  LoopTrees::node_t * tree,
  std::pair<std::set<Data *>, std::set<Data *> > & data_rw,
  const std::set<Data *> & datas
) const {
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

        assert(assign_op != NULL xor compound_assign_op != NULL); // FIXME expression statement are not always made of assignement

        if (assign_op != NULL || compound_assign_op != NULL) {
          append_access(lhs_exp, data_rw.second, datas); // add access in lhs to write list
          append_access(rhs_exp, data_rw.first,  datas); // add access in rhs to read list
        }
        if (compound_assign_op != NULL) {
          append_access(lhs_exp, data_rw.first,  datas); // add access in lhs to read list
        }

      }
      else assert(false); // FIXME expression statement are not always made of binary op
    }
    else assert(false); // FIXME case other than expression statement (replacing the set of leave by a FIFO)
  }
}

void DataFlow::apply(
  const std::set<Data *> & datas_in_,
  const std::set<Data *> & datas_out_,
  const std::set<Data *> & datas_local_,
  const std::list<Kernel *> & kernels,
  const std::map<Kernel *, LoopTrees::node_t *> & kernels_map
) const {
  assert(kernels.size() > 0);

  std::set<Data *> datas;
    datas.insert(datas_in_.begin(), datas_in_.end());
    datas.insert(datas_out_.begin(), datas_out_.end());
    datas.insert(datas_local_.begin(), datas_local_.end());

  // 0 - check preconditions

  {
    // TODO check that Datas are unique:
    // TODO   * One Data per SgSymbol
  }

  // 1 - Compute read/write for each kernel

  std::map<Kernel *, std::pair<std::set<Data *>, std::set<Data *> > > data_rw_map;
  {
    std::list<Kernel *>::const_iterator it_kernel;
    for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      assert(kernel != NULL);

      std::map<Kernel *, LoopTrees::node_t *>::const_iterator it_tree = kernels_map.find(kernel);
      assert(it_tree != kernels_map.end());
      LoopTrees::node_t * tree = it_tree->second;

      std::pair<std::set<Data *>, std::set<Data *> > & data_rw = data_rw_map.insert(
          std::pair<Kernel *, std::pair<std::set<Data *>, std::set<Data *> > >(kernel, std::pair<std::set<Data *>, std::set<Data *> >())
        ).first->second;

      compute_read_write(tree, data_rw, datas);
    }
  }

  // 2 - Propagate

  std::map<Kernel *, std::pair<std::set<Data *>, std::set<Data *> > > data_io_map;
  {
    std::set<Data *> datas_in(datas_in_.begin(), datas_in_.end());
    std::set<Data *> datas_out(datas_out_.begin(), datas_out_.end());
    std::map<Kernel *, std::set<Data *> > data_access_map;

    std::list<Kernel *>::const_iterator it_kernel;
    for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      assert(kernel != NULL);

      std::map<Kernel *, std::pair<std::set<Data *>, std::set<Data *> > >::const_iterator it_data_rw = data_rw_map.find(kernel);
      assert(it_data_rw != data_rw_map.end());
      const std::pair<std::set<Data *>, std::set<Data *> > & data_rw = it_data_rw->second;

      std::pair<std::set<Data *>, std::set<Data *> > & data_io = data_io_map.insert(
          std::pair<Kernel *, std::pair<std::set<Data *>, std::set<Data *> > >(kernel, std::pair<std::set<Data *>, std::set<Data *> >())
        ).first->second;

      std::set<Data *> & data_access = data_access_map.insert(std::pair<Kernel *, std::set<Data *> >(kernel, std::set<Data *>())).first->second;

      // => data_access = data_rw.first union data_rw.second
      Data::set_union(data_access, data_rw.first, data_rw.second);

      // => data_io.first = datas_in inter data_access
      Data::set_intersection(data_io.first, datas_in, data_access);

      // => datas_in = datas_in - data_io.first
      Data::set_remove(datas_in, data_io.first);
    }

    std::list<Kernel *>::const_reverse_iterator rit_kernel;
    for (rit_kernel = kernels.rbegin(); rit_kernel != kernels.rend(); rit_kernel++) {
      Kernel * kernel = *rit_kernel;
      assert(kernel != NULL);

      std::map<Kernel *, std::pair<std::set<Data *>, std::set<Data *> > >::iterator it_data_io = data_io_map.find(kernel);
      assert(it_data_io != data_io_map.end());
      std::pair<std::set<Data *>, std::set<Data *> > & data_io = it_data_io->second;

      std::map<Kernel *, std::set<Data *> >::const_iterator it_data_access = data_access_map.find(kernel);
      assert(it_data_access != data_access_map.end());
      const std::set<Data *> & data_access = it_data_access->second;

      // => data_io.second = datas_out inter data_access
      Data::set_intersection(data_io.second, datas_out, data_access);

      // => datas_out = datas_out - data_io.second
      Data::set_remove(datas_out, data_io.second);
    }
  }

  // 3 - Compute the flow of data

  // TODO

  // 4 - Finish

  {
    std::list<Kernel *>::const_iterator it_kernel;
    for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      assert(kernel != NULL);

      std::map<Kernel *, std::pair<std::set<Data *>, std::set<Data *> > >::iterator it_data_io = data_io_map.find(kernel);
      assert(it_data_io != data_io_map.end());
      std::pair<std::set<Data *>, std::set<Data *> > & data_io = it_data_io->second;

      kernel->setDataflow(data_io.first, data_io.second);
    }
  }
}

}

}

