
#ifndef __KLT_DATA_FLOW_HPP__
#define __KLT_DATA_FLOW_HPP__

#include <list>

namespace KLT {

template <class Annotation> class Data;
template <class Annotation, class Language, class Runtime> class Kernel;
template <class Annotation> class LoopTrees;

/*!
 * \addtogroup grp_klt_dataflow
 * @{
*/

template <class Annotation, class Language, class Runtime>
class DataFlow {
  public:
    virtual void generateFlowSets(
      const LoopTrees<Annotation> & loop_trees,
      const std::list<Kernel<Annotation, Language, Runtime> *> & kernels
    ) const;
};

template <class Annotation>
void append_access(
    SgExpression * exp,
    std::set<Data<Annotation> *> & access_set,
    const std::set<Data<Annotation> *> & datas
) {
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(exp);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();
    assert(var_sym != NULL);

    Data<Annotation> * data = NULL;
    typename std::set<Data<Annotation> *>::const_iterator it_data;
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

template <class Annotation, class Language, class Runtime>
void compute_read_write(
    typename LoopTrees<Annotation>::node_t * tree,
    typename Kernel<Annotation, Language, Runtime>::dataflow_t & data_flow,
    const std::set<Data<Annotation> *> & datas
) {
  std::set<SgStatement *> leaves;
  collectLeaves<Annotation>(tree, leaves);

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
          append_access<Annotation>(lhs_exp, data_flow.write, datas); // add access in lhs to write set
          append_access<Annotation>(rhs_exp, data_flow.read,  datas); // add access in rhs to read set
        }
        if (compound_assign_op != NULL) {
          append_access<Annotation>(lhs_exp, data_flow.read,  datas); // add access in lhs to read set
        }
      }
      else assert(false); // FIXME expression statement are not always made of binary op
    }
    else assert(false); // FIXME case other than expression statement (replacing the set of leave by a FIFO)
  }

  Data<Annotation>::set_union(data_flow.datas, data_flow.read, data_flow.write);
}

template <class Annotation, class Language, class Runtime>
void DataFlow<Annotation, Language, Runtime>::generateFlowSets(
  const LoopTrees<Annotation> & loop_trees,
  const std::list<Kernel<Annotation, Language, Runtime> *> & kernels
) const {
  assert(kernels.size() > 0);

  typename std::list<Kernel<Annotation, Language, Runtime> *>::const_iterator it_kernel;
  typename std::list<Kernel<Annotation, Language, Runtime> *>::const_reverse_iterator rit_kernel;
  typename std::set<Data<Annotation> *>::iterator it_data;

  std::set<Data<Annotation> *> datas(loop_trees.getDatas());
  std::set<Data<Annotation> *> datas_in;
  std::set<Data<Annotation> *> datas_out;
  for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
    assert(*it_data != NULL);
    if ((*it_data)->isFlowIn())
      datas_in.insert(*it_data);
    if ((*it_data)->isFlowOut())
      datas_out.insert(*it_data);
  }

  // 1 - Compute read/write sets for each kernel and set of all accessed data
  
  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    Kernel<Annotation, Language, Runtime> * kernel = *it_kernel;
    typename Kernel<Annotation, Language, Runtime>::dataflow_t & data_flow = kernel->getDataflow();
    compute_read_write<Annotation, Language, Runtime>(kernel->getRoot(), data_flow, datas);
  }

  // 2 - Propagate:
  //       * data is flow-in  in a kernel if it is not flow-in  for a previous  kernel and it is used in this kernel (R/W)
  //       * data is flow-out in a kernel if it is not flow-out for a following kernel and it is used in this kernel (R/W)

  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    Kernel<Annotation, Language, Runtime> * kernel = *it_kernel;
    typename Kernel<Annotation, Language, Runtime>::dataflow_t & data_flow = kernel->getDataflow();

    Data<Annotation>::set_intersection(data_flow.flow_in, datas_in, data_flow.datas);

    Data<Annotation>::set_remove(datas_in, data_flow.flow_in);
  }

  for (rit_kernel = kernels.rbegin(); rit_kernel != kernels.rend(); rit_kernel++) {
    Kernel<Annotation, Language, Runtime> * kernel = *rit_kernel;
    typename Kernel<Annotation, Language, Runtime>::dataflow_t & data_flow = kernel->getDataflow();

    Data<Annotation>::set_intersection(data_flow.flow_out, datas_out, data_flow.datas);

    Data<Annotation>::set_remove(datas_out, data_flow.flow_out);
  }

  // 3 - Compute the flow of data

  // TODO last set of equations...
}

/** @} */

}

#endif /* __KLT_DATA_FLOW_HPP__ */

