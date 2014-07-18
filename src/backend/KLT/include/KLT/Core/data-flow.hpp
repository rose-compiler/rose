
#ifndef __KLT_DATA_FLOW_HPP__
#define __KLT_DATA_FLOW_HPP__

#include <map>
#include <list>
#include <set>
#include <vector>

#include "sage3basic.h"

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
    struct data_access_t {
      Data<Annotation> * data;
      std::vector<SgExpression *> subscripts;
    };

    struct accesses_list_t {
      std::vector<data_access_t> reads;
      std::vector<data_access_t> writes;
    };

    struct context_t {
      std::set<Data<Annotation> *> datas;
      std::set<Data<Annotation> *> datas_in;
      std::set<Data<Annotation> *> datas_out;
      std::map<typename LoopTrees<Annotation>::node_t *, accesses_list_t > accesses_map;
    };

  private:
    void append_access(
      SgExpression * exp,
      std::vector<data_access_t> & access_set,
      const context_t & context
    ) const;

  public:
    void createContextFromLoopTree(
      const LoopTrees<Annotation> & loop_trees,
      context_t & context
    ) const;

    void markSplittedData(
      const context_t & context
    ) const;

    void generateFlowSets(
      const std::list<Kernel<Annotation, Language, Runtime> *> & kernels,
      const context_t & context
    ) const;
};

template <class Annotation, class Language, class Runtime>
void DataFlow<Annotation, Language, Runtime>::append_access(
  SgExpression * exp,
  std::vector<data_access_t> & access_set,
  const context_t & context
) const {
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(exp);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();
    assert(var_sym != NULL);

    Data<Annotation> * data = NULL;
    typename std::set<Data<Annotation> *>::const_iterator it_data;
    for (it_data = context.datas.begin(); it_data != context.datas.end(); it_data++)
      if ((*it_data)->getVariableSymbol() == var_sym) {
        data = *it_data;
        break;
      }
    if (data == NULL)
      continue;

    access_set.push_back(data_access_t());
    access_set.back().data = data;
    SgPntrArrRefExp * parent = isSgPntrArrRefExp(var_ref->get_parent());
    while (parent != NULL) {
      access_set.back().subscripts.push_back(parent->get_rhs_operand_i());
      parent = isSgPntrArrRefExp(parent->get_parent());
    }
    assert(access_set.back().subscripts.size() == data->getSections().size());
  }
}

template <class Annotation, class Language, class Runtime>
void DataFlow<Annotation, Language, Runtime>::createContextFromLoopTree(
  const LoopTrees<Annotation> & loop_trees,
  context_t & context
) const {
  assert(context.datas.empty());
  assert(context.datas_in.empty());
  assert(context.datas_out.empty());
  assert(context.accesses_map.empty());

  typename std::set<Data<Annotation> *>::iterator it_data;

  const std::vector<Data<Annotation> *> & data_vect = loop_trees.getDatas();
  context.datas.insert(data_vect.begin(), data_vect.end());
  for (it_data = context.datas.begin(); it_data != context.datas.end(); it_data++) {
    assert(*it_data != NULL);
    if ((*it_data)->isFlowIn())
      context.datas_in.insert(*it_data);
    if ((*it_data)->isFlowOut())
      context.datas_out.insert(*it_data);
  }

  const std::vector<typename LoopTrees<Annotation>::node_t *> & nodes = loop_trees.getNodes();
  typename std::vector<typename LoopTrees<Annotation>::node_t *>::const_iterator it_node;
  for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) {
    typename LoopTrees<Annotation>::cond_t * cond = dynamic_cast<typename LoopTrees<Annotation>::cond_t *>(*it_node);
    typename LoopTrees<Annotation>::stmt_t * stmt = dynamic_cast<typename LoopTrees<Annotation>::stmt_t *>(*it_node);

    if (cond == NULL && stmt == NULL) continue;

    typename std::map<typename LoopTrees<Annotation>::node_t *, accesses_list_t>::iterator it_access = 
          context.accesses_map.insert(std::pair<typename LoopTrees<Annotation>::node_t *, accesses_list_t>(*it_node, accesses_list_t())).first;

    std::vector<data_access_t> & read  = it_access->second.reads;
    std::vector<data_access_t> & write = it_access->second.writes;

    if (cond != NULL) {
      assert(false); /// \todo
    }
    else if (stmt != NULL) {
      SgExprStatement * expr_stmt = isSgExprStatement(stmt->statement);
      SgVariableDeclaration * var_decl = isSgVariableDeclaration(stmt->statement);
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
            append_access(lhs_exp, write, context); // add access in lhs to write set
            append_access(rhs_exp, read,  context); // add access in rhs to read set
          }
          if (compound_assign_op != NULL) {
            append_access(lhs_exp, read,  context); // add access in lhs to read set
          }
        }
        else assert(false); // FIXME expression statement are not always made of binary op
      }
      else if (var_decl != NULL) {
        assert(var_decl->get_variables().size() == 1);
        SgInitializedName * init_name = isSgInitializedName(var_decl->get_variables()[0]);
        assert(init_name != NULL);
        SgInitializer * init = isSgInitializer(init_name->get_initptr());
        if (init != NULL) {
          SgAssignInitializer * assign_init = isSgAssignInitializer(init);
          if (assign_init != NULL) {
            SgExpression * exp = assign_init->get_operand_i();
            assert(exp != NULL);
            append_access(exp, read, context);
          }
          else assert(false);
        }
      }
      else assert(false);
    }
  }
}

template <class Annotation, class Language, class Runtime>
void DataFlow<Annotation, Language, Runtime>::generateFlowSets(
  const std::list<Kernel<Annotation, Language, Runtime> *> & kernels,
  const context_t & context
) const {
  assert(!context.datas.empty());
  assert(!context.accesses_map.empty());

  assert(kernels.size() > 0);

  typename std::list<Kernel<Annotation, Language, Runtime> *>::const_iterator it_kernel;
  typename std::list<Kernel<Annotation, Language, Runtime> *>::const_reverse_iterator rit_kernel;
  typename std::set<Data<Annotation> *>::iterator it_data;

  // 1 - Aggregate read/write sets for each kernel

  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    Kernel<Annotation, Language, Runtime> * kernel = *it_kernel;
    typename Kernel<Annotation, Language, Runtime>::dataflow_t & data_flow = kernel->getDataflow();
    const std::vector<typename LoopTrees<Annotation>::node_t *> & nodes = kernel->getNodes();
    typename std::vector<typename LoopTrees<Annotation>::node_t *>::const_iterator it_node;

    for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) {
      typename std::map<typename LoopTrees<Annotation>::node_t *, accesses_list_t>::const_iterator it_access_list = context.accesses_map.find(*it_node);
      if (it_access_list == context.accesses_map.end()) continue;
      typename std::vector<data_access_t>::const_iterator it_access;
      for (it_access = it_access_list->second.reads.begin(); it_access != it_access_list->second.reads.end(); it_access++)
        data_flow.datas.insert(it_access->data);
      for (it_access = it_access_list->second.writes.begin(); it_access != it_access_list->second.writes.end(); it_access++)
        data_flow.datas.insert(it_access->data);
    }
  }

  // 2 - Propagate:
  //       * data is flow-in  in a kernel if it is not flow-in  for a previous  kernel and it is used in this kernel (R/W)
  //       * data is flow-out in a kernel if it is not flow-out for a following kernel and it is used in this kernel (R/W)

  std::set<Data<Annotation> *> datas_in(context.datas_in);
  std::set<Data<Annotation> *> datas_out(context.datas_out);

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

