
#include "KLT/dlx-openacc.hpp"
#include "KLT/loop-trees.hpp"
#include "KLT/generator.hpp"
#include "KLT/kernel.hpp"
#include "KLT/data.hpp"
#include "KLT/iteration-mapper.hpp"
#include "KLT/language-opencl.hpp"
#include "KLT/runtime-openacc.hpp"
#include "KLT/mfb-klt.hpp"
#include "KLT/mfb-acc-ocl.hpp"
#include "KLT/utils.hpp"

#include "MFB/Sage/function-declaration.hpp"

#include "sage3basic.h"

//! This helper function return non-null value iff the expression found is statically defined integer.
SgExpression * parseExpressionOrLabel() {
  SgExpression * exp = NULL;
  if (AstFromString::afs_match_additive_expression()) {
    exp = isSgExpression(AstFromString::c_parsed_node);
    assert(exp != NULL);
    /// \todo Is it a statically defined integer?
  }
  else if (AstFromString::afs_match_identifier()) {
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);
    // We don't save in this case as it implies a dynamically determined value
  }
  else assert(false);

  return exp;
}

namespace DLX {

template <>
void KLT_Annotation<OpenACC::language_t>::parseRegion(std::vector<DLX::KLT_Annotation<OpenACC::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<OpenACC::language_t> & annotation = container.back();

  switch (annotation.clause->kind) {
    case OpenACC::language_t::e_acc_clause_if:
      assert(false); /// \todo
      break;
    case OpenACC::language_t::e_acc_clause_async:
      assert(false); /// \todo
      break;
    case OpenACC::language_t::e_acc_clause_num_gangs:
      KLT::ensure('(');
      ((Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_gangs> *)annotation.clause)->parameters.exp
                = parseExpressionOrLabel();
      KLT::ensure(')');
      break;
    case OpenACC::language_t::e_acc_clause_num_workers:
      KLT::ensure('(');
      ((Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_workers> *)annotation.clause)->parameters.exp
                = parseExpressionOrLabel();
      KLT::ensure(')');
      break;
    case OpenACC::language_t::e_acc_clause_vector_length:
      KLT::ensure('(');
      ((Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector_length> *)annotation.clause)->parameters.exp
                = parseExpressionOrLabel();
      KLT::ensure(')');
      break;
    case OpenACC::language_t::e_acc_clause_reduction:
      assert(false); /// \todo
      break;
    default:
      assert(false);
  }
}

template <>
void KLT_Annotation<OpenACC::language_t>::parseData(std::vector<DLX::KLT_Annotation<OpenACC::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<OpenACC::language_t> & annotation = container.back();

  switch (annotation.clause->kind) {
    case OpenACC::language_t::e_acc_clause_copy:
    case OpenACC::language_t::e_acc_clause_copyin:
    case OpenACC::language_t::e_acc_clause_copyout:
    case OpenACC::language_t::e_acc_clause_create:
    case OpenACC::language_t::e_acc_clause_present:
    case OpenACC::language_t::e_acc_clause_present_or_copy:
    case OpenACC::language_t::e_acc_clause_present_or_copyin:
    case OpenACC::language_t::e_acc_clause_present_or_copyout:
    case OpenACC::language_t::e_acc_clause_present_or_create:
      // Nothing to do as in the LoopTree format, clauses are applied to each data (in directive format the clause encompass multiple data)
      break;
    default:
      assert(false);
  }
}

template <>
void KLT_Annotation<OpenACC::language_t>::parseLoop(std::vector<DLX::KLT_Annotation<OpenACC::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<OpenACC::language_t> & annotation = container.back();

  switch (annotation.clause->kind) {
    case OpenACC::language_t::e_acc_clause_gang:
    case OpenACC::language_t::e_acc_clause_worker:
    case OpenACC::language_t::e_acc_clause_vector:
    case OpenACC::language_t::e_acc_clause_seq:
    case OpenACC::language_t::e_acc_clause_independent:
      // None of these clauses take any parameters
      break;
    default:
      assert(false);
  }
}

}

namespace KLT {

template <>
bool LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t::isDistributed() const {
  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = annotations.begin(); it != annotations.end(); it++) {
    if (   it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_gang
        || it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_worker
        || it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_vector
    ) return true;
  }
  return false;
}

template <>
unsigned long Generator<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC,
  MFB::KLT_Driver
>::createFile() {
  return p_sage_driver.add(boost::filesystem::path(p_file_name));
}

template <>
unsigned long Kernel<DLX::KLT_Annotation<DLX::OpenACC::language_t>, Language::OpenCL, Runtime::OpenACC>::id_cnt = 0;

template <>
bool Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::isFlowIn() const {
  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = annotations.begin(); it != annotations.end(); it++) {
    if (   it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_copy
        || it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_copyin
    ) return true;
  }
  return false;
}

template <>
bool Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::isFlowOut() const {
  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = annotations.begin(); it != annotations.end(); it++) {
    if (   it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_copy
        || it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_copyout
    ) return true;
  }
  return false;
}

template <>
void IterationMapper<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC
>::computeValidShapes(
  LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t * loop,
  std::vector<Runtime::OpenACC::loop_shape_t *> & shapes
) const {
  if (!loop->isDistributed()) return;

  long gang = 1;
  long worker = 1;
  long vector = 1;

  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = loop->annotations.begin(); it != loop->annotations.end(); it++) {
    /// \todo Does not support static values for gang.worker,vector
    if (it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_gang  ) gang   = 0;
    if (it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_worker) worker = 0;
    if (it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_vector) vector = 0;
  }

  if (gang > 1)    assert(false); /// \todo Not yet supported by the generator
  if (vector > 1)  assert(false); /// \todo Not yet supported by the generator
  if (vector != 1) assert(false); /// \todo Not yet supported by the generator

  /// \todo  Generate static tiles, currently static tiles will be simulated by dynamic one. 
  for (unsigned i = 0; i < 2; i++) {
    if (gang == 1 && worker == 1)
      shapes.push_back(new Runtime::OpenACC::loop_shape_t((1 >> i), 1, 1, 1, 1, 1, 1));

    for (unsigned j = 0; j < 2; j++) {
      if (gang == 0 && worker == 1)
        shapes.push_back(new Runtime::OpenACC::loop_shape_t((1 >> i), 0, (1 >> j), 1, 1, 1, 1));

      if (gang == 1 && worker == 0)
        shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1, (1 >> i), 0, (1 >> j), 1, 1));

      for (unsigned k = 0; k < 2; k++) {
        if (gang == 0 && worker == 0)
          shapes.push_back(new Runtime::OpenACC::loop_shape_t((1 >> i), 0, (1 >> j), 0, (1 >> k), 1, 1));
      }
    }
  }
}

template <>
SgFunctionParameterList * createParameterList<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC
>(
  Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>,
    Language::OpenCL,
    Runtime::OpenACC
  > * kernel
) {
  const std::list<SgVariableSymbol *> & params = kernel->getArguments().parameters;
  const std::list<SgVariableSymbol *> & scalars = kernel->getArguments().scalars;
  const std::list<Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *> & datas = kernel->getArguments().datas;
  unsigned long data_type_modifer_ = SgTypeModifier::e_ocl_global__;

  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list<Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *>::const_iterator it_data;

  SgTypeModifier::type_modifier_enum data_type_modifer = (SgTypeModifier::type_modifier_enum)data_type_modifer_;

  SgFunctionParameterList * result = SageBuilder::buildFunctionParameterList();

  // ******************

  for (it_var_sym = params.begin(); it_var_sym != params.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type =  param_sym->get_type();

    result->append_arg(SageBuilder::buildInitializedName("param_" + param_name, param_type, NULL));
  }

  // ******************

  for (it_var_sym = scalars.begin(); it_var_sym != scalars.end(); it_var_sym++) {
    SgVariableSymbol * scalar_sym = *it_var_sym;
    std::string scalar_name = scalar_sym->get_name().getString();
    SgType * scalar_type = scalar_sym->get_type();

    result->append_arg(SageBuilder::buildInitializedName("scalar_" + scalar_name, scalar_type, NULL));
  }

  // ******************

  for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
    Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();
    std::string data_name = data_sym->get_name().getString();

    SgType * base_type = data->getBaseType();
    SgType * field_type = SageBuilder::buildPointerType(base_type);

    switch (data_type_modifer) {
      case SgTypeModifier::e_default:
        break;
      case SgTypeModifier::e_ocl_global__:
      {
        SgModifierType * modif_type = SageBuilder::buildModifierType(field_type);
        modif_type->get_typeModifier().setOpenclGlobal();
        field_type = modif_type;
        break;
      }
      default:
        assert(false);
    }

    result->append_arg(SageBuilder::buildInitializedName("data_" + data_name, field_type, NULL));
  }

  result->append_arg(SageBuilder::buildInitializedName(
    "context",
    Runtime::OpenACC::runtime_device_context_symbol->get_declaration()->get_type(),
    NULL
  ));
  
  return result;
}

template <>
SgStatement * generateStatement<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC
> (
  LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::stmt_t * stmt,
  const Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>, Language::OpenCL, Runtime::OpenACC
  >::local_symbol_maps_t & local_symbol_maps,
  bool flatten_array_ref
) {
  SgStatement * result = SageInterface::copyStatement(stmt->statement);

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;
  std::map<Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *, SgVariableSymbol *>::const_iterator it_data_to_local;

  std::map<SgVariableSymbol *, SgVariableSymbol *> data_sym_to_local;
  std::map<SgVariableSymbol *, Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *> data_sym_to_data;

  for (it_data_to_local = local_symbol_maps.datas.begin(); it_data_to_local != local_symbol_maps.datas.end(); it_data_to_local++) {
    Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > * data = it_data_to_local->first;
    SgVariableSymbol * data_sym = it_data_to_local->first->getVariableSymbol();
    SgVariableSymbol * local_sym = it_data_to_local->second;

    data_sym_to_local.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, local_sym));
    data_sym_to_data.insert(std::pair<SgVariableSymbol *, Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *>(data_sym, data));
  }

  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  if (flatten_array_ref) {
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
      SgVarRefExp * var_ref = *it_var_ref;
      SgVariableSymbol * var_sym = var_ref->get_symbol();

      std::map<SgVariableSymbol *, Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *>::const_iterator it_data_sym_to_data = data_sym_to_data.find(var_sym);
      if (it_data_sym_to_data == data_sym_to_data.end()) continue; // Not a variable reference to a Data

      Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > * data = it_data_sym_to_data->second;

      if (data->getSections().size() <= 1) continue; // No need for flattening

      SgPntrArrRefExp * arr_ref = isSgPntrArrRefExp(var_ref->get_parent());
      SgPntrArrRefExp * top_arr_ref = NULL;
      std::list<SgExpression *> subscripts;
      while (arr_ref != NULL) {
        top_arr_ref = arr_ref;
        subscripts.push_back(arr_ref->get_rhs_operand_i());
        arr_ref = isSgPntrArrRefExp(arr_ref->get_parent());
      }
      assert(top_arr_ref != NULL);
      assert(subscripts.size() == data->getSections().size());

      std::list<SgExpression *>::const_iterator it_subscript;
      SgExpression * subscript = SageInterface::copyExpression(subscripts.front());
      subscripts.pop_front();
      unsigned int cnt = 0;
      for (it_subscript = subscripts.begin(); it_subscript != subscripts.end(); it_subscript++) {
        SgExpression * dim_size = SageInterface::copyExpression(data->getSections()[cnt++].second);
        subscript = SageBuilder::buildMultiplyOp(subscript, dim_size);
        subscript = SageBuilder::buildAddOp(subscript, SageInterface::copyExpression(*it_subscript));
      }

      SageInterface::replaceExpression(top_arr_ref, SageBuilder::buildPntrArrRefExp(SageInterface::copyExpression(var_ref), subscript));
    }
  }

  var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();

    SgVariableSymbol * local_sym = NULL;
    it_sym_to_local = local_symbol_maps.parameters.find(var_sym);
    if (it_sym_to_local != local_symbol_maps.parameters.end())
      local_sym = it_sym_to_local->second;

    it_sym_to_local = local_symbol_maps.scalars.find(var_sym);
    if (it_sym_to_local != local_symbol_maps.scalars.end()) {
      assert(local_sym == NULL);

      local_sym = it_sym_to_local->second;
    }

    it_sym_to_local = data_sym_to_local.find(var_sym);
    if (it_sym_to_local != data_sym_to_local.end()) {
      assert(local_sym == NULL);

      local_sym = it_sym_to_local->second;
    }

    it_sym_to_local = local_symbol_maps.iterators.find(var_sym);
    if (it_sym_to_local != local_symbol_maps.iterators.end()) {
      assert(local_sym == NULL);

      local_sym = it_sym_to_local->second;
    }

    assert(local_sym != NULL); // implies VarRef to an unknown variable symbol

    SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym));
  }

  assert(result != NULL);

  return result;
}

SgExpression * translateConstExpression(
  SgExpression * expr,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
) {
  SgExpression * result = SageInterface::copyExpression(expr);

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;

  if (isSgVarRefExp(result)) {
    // Catch an issue when reading looptree from file. In this case, 'expr' may not have a valid parent.
    // If 'expr' is a SgVarRefExp, it causes an assertion to fail in SageInterface::replaceExpression

    SgVarRefExp * var_ref = (SgVarRefExp *)result;

    SgVariableSymbol * var_sym = var_ref->get_symbol();

    SgVariableSymbol * local_sym = NULL;

    it_sym_to_local = param_to_local.find(var_sym);
    if (it_sym_to_local != param_to_local.end())
      local_sym = it_sym_to_local->second;

    it_sym_to_local = iter_to_local.find(var_sym);
    if (it_sym_to_local != iter_to_local.end()) {
      assert(local_sym == NULL); // implies VarRef to a variable symbol which is both parameter and iterator... It does not make sense!

      local_sym = it_sym_to_local->second;
    }

    assert(local_sym != NULL); // implies VarRef to an unknown variable symbol (neither parameter or iterator)

    return SageBuilder::buildVarRefExp(local_sym);
  }
  
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();

    SgVariableSymbol * local_sym = NULL;

    it_sym_to_local = param_to_local.find(var_sym);
    if (it_sym_to_local != param_to_local.end())
      local_sym = it_sym_to_local->second;

    it_sym_to_local = iter_to_local.find(var_sym);
    if (it_sym_to_local != iter_to_local.end()) {
      assert(local_sym == NULL); // implies VarRef to a variable symbol which is both parameter and iterator... It does not make sense!

      local_sym = it_sym_to_local->second;
    }

    assert(local_sym != NULL); // implies VarRef to an unknown variable symbol (neither parameter or iterator)

    SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym));
  }

  return result;
}

std::pair<SgStatement *, SgScopeStatement *> makeTile(
  SgVariableSymbol * tile_iterator,
  SgExpression * base,
  unsigned tile_id,
  SgExpression * runtime_loop_desc,
  const Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>, Language::OpenCL, Runtime::OpenACC
  >::local_symbol_maps_t & local_symbol_maps
) {
  
/*
      for (it_loop_0_tile_2  = it_loop_0_worker;
           it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
           it_loop_0_tile_2 += ctx->loops[0].tiles[e_tile_2].stride)
*/

  SgExpression * lower_bound = base;
  SgExpression * upper_bound = SageBuilder::buildAddOp(
                                 SageInterface::copyExpression(base),
                                 SageBuilder::buildDotExp(
                                   SageBuilder::buildDotExp(
                                     runtime_loop_desc,
                                     SageBuilder::buildPntrArrRefExp(
                                       SageBuilder::buildVarRefExp(Runtime::OpenACC::runtime_kernel_loop_symbols.tiles_symbol),
                                       SageBuilder::buildIntVal(tile_id)
                                     )
                                   ),
                                   SageBuilder::buildVarRefExp(Runtime::OpenACC::runtime_kernel_loop_symbols.tiles_length_symbol)
                                 )
                               ); // 'base' + 'runtime_loop_desc'.tiles['tile_id'].length

  SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(tile_iterator), lower_bound));
  SgExprStatement * test_stmt  = SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(SageBuilder::buildVarRefExp(tile_iterator), upper_bound));;
  SgExpression * inc_expr = SageBuilder::buildPlusAssignOp(
                              SageBuilder::buildVarRefExp(tile_iterator),
                              SageBuilder::buildDotExp(
                                SageBuilder::buildDotExp(
                                  SageInterface::copyExpression(runtime_loop_desc),
                                  SageBuilder::buildPntrArrRefExp(
                                    SageBuilder::buildVarRefExp(Runtime::OpenACC::runtime_kernel_loop_symbols.tiles_symbol),
                                    SageBuilder::buildIntVal(tile_id)
                                  )
                                ),
                                SageBuilder::buildVarRefExp(Runtime::OpenACC::runtime_kernel_loop_symbols.tiles_stride_symbol)
                              )
                            ); // 'tile_iterator' += 'runtime_loop_desc'.tiles['tile_id'].stride

  SgBasicBlock * for_body = SageBuilder::buildBasicBlock();
  SgForStatement * for_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, inc_expr, for_body);

  return std::pair<SgStatement *, SgScopeStatement *>(for_stmt, for_body);
}

template <>
std::pair<SgStatement *, SgScopeStatement *> generateLoops<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC
> (
  LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t * loop,
  unsigned & loop_id,
  std::vector<Runtime::OpenACC::a_loop> & loop_descriptors,
  Runtime::OpenACC::loop_shape_t * shape,
  const Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>, Language::OpenCL, Runtime::OpenACC
  >::local_symbol_maps_t & local_symbol_maps
) {
  if (!loop->isDistributed()) {
    assert(shape == NULL);

    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local = local_symbol_maps.iterators.find(loop->iterator);
    assert(it_sym_to_local != local_symbol_maps.iterators.end());
    SgVariableSymbol * local_it_sym = it_sym_to_local->second;

    SgExpression * lower_bound = translateConstExpression(loop->lower_bound, local_symbol_maps.parameters, local_symbol_maps.iterators);
    SgExpression * upper_bound = translateConstExpression(loop->upper_bound, local_symbol_maps.parameters, local_symbol_maps.iterators);

    SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(local_it_sym), lower_bound));
    SgExprStatement * test_stmt  = SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(SageBuilder::buildVarRefExp(local_it_sym), upper_bound));;
    SgExpression * inc_expr = SageBuilder::buildPlusAssignOp(
                                SageBuilder::buildVarRefExp(local_it_sym),
                                SageBuilder::buildIntVal(1) /// \todo add stride expression to LoopTrees' loops
                              );

    SgBasicBlock * for_body = SageBuilder::buildBasicBlock();
    SgForStatement * for_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, inc_expr, for_body);

    return std::pair<SgStatement *, SgScopeStatement *>(for_stmt, for_body);
  }
  else {
    assert(shape != NULL);

    Runtime::OpenACC::a_loop loop_desc;
    std::pair<SgStatement *, SgScopeStatement *> result(NULL, NULL);
    SgExpression * runtime_loop_desc = SageBuilder::buildArrowExp(
                                         SageBuilder::buildVarRefExp(local_symbol_maps.context),
                                         SageBuilder::buildPntrArrRefExp(
                                           SageBuilder::buildVarRefExp(Runtime::OpenACC::runtime_context_symbols.loops_symbol),
                                           SageBuilder::buildIntVal(loop_id)
                                         )
                                       ); // ctx->loops['loop_id']
    SgExpression * base = SageBuilder::buildDotExp(
                            SageBuilder::buildDotExp(
                              SageInterface::copyExpression(runtime_loop_desc),
                              SageBuilder::buildVarRefExp(Runtime::OpenACC::runtime_kernel_loop_symbols.original_symbol)
                            ),
                            SageBuilder::buildVarRefExp(Runtime::OpenACC::runtime_loop_desc_symbols.lower_symbol)
                          ); // ctx->loops[0].original.lower

    if (shape->tile_0 != 1) {
      std::pair<SgStatement *, SgScopeStatement *> tile = makeTile(
        shape->iterators[0], base, 0, SageInterface::copyExpression(runtime_loop_desc), local_symbol_maps
      );
      base = SageBuilder::buildVarRefExp(shape->iterators[0]);

      if (result.first == NULL && result.second == NULL) {
        result.first = tile.first;
        result.second = tile.second;
      }
      else {
        assert(result.first != NULL && result.second != NULL);

        SageInterface::appendStatement(tile.first, result.second);
        result.second = tile.second;
      }
    }

    if (shape->gang != 1) {
      SgStatement * set_gang_it = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
        SageBuilder::buildVarRefExp(shape->iterators[1]),
        SageBuilder::buildFunctionCallExp(
          SageBuilder::buildFunctionRefExp(Runtime::OpenACC::runtime_device_function_symbols.gang_iter_symbol),
          SageBuilder::buildExprListExp(
            SageBuilder::buildVarRefExp(local_symbol_maps.context),
            SageBuilder::buildIntVal(loop_id),
            base
          )
        )
      ));
      if (result.first == NULL && result.second == NULL) {
        result.second = SageBuilder::buildBasicBlock();
        result.first = result.second;
      }
      base = SageBuilder::buildVarRefExp(shape->iterators[1]);
      SageInterface::appendStatement(set_gang_it, result.second);
    }

    if (shape->tile_1 != 1) {
      std::pair<SgStatement *, SgScopeStatement *> tile = makeTile(
        shape->iterators[2], base, 2, SageInterface::copyExpression(runtime_loop_desc), local_symbol_maps
      );
      base = SageBuilder::buildVarRefExp(shape->iterators[2]);

      if (result.first == NULL && result.second == NULL) {
        result.first = tile.first;
        result.second = tile.second;
      }
      else {
        assert(result.first != NULL && result.second != NULL);

        SageInterface::appendStatement(tile.first, result.second);
        result.second = tile.second;
      }
    }

    if (shape->worker != 1) {
      SgStatement * set_worker_it = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
        SageBuilder::buildVarRefExp(shape->iterators[3]),
        SageBuilder::buildFunctionCallExp(
          SageBuilder::buildFunctionRefExp(Runtime::OpenACC::runtime_device_function_symbols.worker_iter_symbol),
          SageBuilder::buildExprListExp(
            SageBuilder::buildVarRefExp(local_symbol_maps.context),
            SageBuilder::buildIntVal(loop_id),
            base
          )
        )
      ));
      if (result.first == NULL && result.second == NULL) {
        result.second = SageBuilder::buildBasicBlock();
        result.first = result.second;
      }
      base = SageBuilder::buildVarRefExp(shape->iterators[3]);
      SageInterface::appendStatement(set_worker_it, result.second);
    }

    if (shape->tile_2 != 1) {
      std::pair<SgStatement *, SgScopeStatement *> tile = makeTile(
        shape->iterators[4], base, 4, SageInterface::copyExpression(runtime_loop_desc), local_symbol_maps
      );
      base = SageBuilder::buildVarRefExp(shape->iterators[4]);

      if (result.first == NULL && result.second == NULL) {
        result.first = tile.first;
        result.second = tile.second;
      }
      else {
        assert(result.first != NULL && result.second != NULL);

        SageInterface::appendStatement(tile.first, result.second);
        result.second = tile.second;
      }
    }

    if (shape->vector == 0) { // Dynamic vector cannot be generated, so they are simulated with a loop...
      std::pair<SgStatement *, SgScopeStatement *> tile = makeTile(
        shape->iterators[5], base, 5, SageInterface::copyExpression(runtime_loop_desc), local_symbol_maps
      );
      base = SageBuilder::buildVarRefExp(shape->iterators[5]);

      if (result.first == NULL && result.second == NULL) {
        result.first = tile.first;
        result.second = tile.second;
      }
      else {
        assert(result.first != NULL && result.second != NULL);

        SageInterface::appendStatement(tile.first, result.second);
        result.second = tile.second;
      }
    }
    else if (shape->vector > 1) {
      assert(false); /// \todo generate vector expression from contained statement, should be done by 'generateStatement'
    }

    if (shape->tile_3 != 1) {
      std::pair<SgStatement *, SgScopeStatement *> tile = makeTile(
        shape->iterators[6], base, 6, SageInterface::copyExpression(runtime_loop_desc), local_symbol_maps
      );
      base = SageBuilder::buildVarRefExp(shape->iterators[6]);

      if (result.first == NULL && result.second == NULL) {
        result.first = tile.first;
        result.second = tile.second;
      }
      else {
        assert(result.first != NULL && result.second != NULL);

        SageInterface::appendStatement(tile.first, result.second);
        result.second = tile.second;
      }
    }

    loop_descriptors.push_back(loop_desc);
    loop_id++;
    assert(loop_descriptors.size() == loop_id);

    return result;
  }

  assert(false);
}

}

namespace MFB {

KLT<Kernel_OpenCL_OpenACC>::object_desc_t::object_desc_t(
  unsigned id_,
  Kernel_OpenCL_OpenACC * kernel_,
  unsigned long file_id_
) :
  id(id_),
  kernel(kernel_),
  file_id(file_id_),
  shapes()
{}

template <>
SgBasicBlock * createLocalDeclarations<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  ::KLT::Language::OpenCL,
  ::KLT::Runtime::OpenACC
>(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  ::KLT::Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>,
    ::KLT::Language::OpenCL,
    ::KLT::Runtime::OpenACC
  >::local_symbol_maps_t & local_symbol_maps,
  const ::KLT::Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>,
    ::KLT::Language::OpenCL,
    ::KLT::Runtime::OpenACC
  >::arguments_t & arguments,
  const std::map<
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *,
    ::KLT::Runtime::OpenACC::loop_shape_t *
  > & loop_shapes
) {
  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *>::const_iterator it_data;

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_param_to_field;
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_scalar_to_field;
  std::map< ::KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *, SgVariableSymbol *>::const_iterator it_data_to_field;

  std::map<
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *,
    ::KLT::Runtime::OpenACC::loop_shape_t *
  >::const_iterator it_loop_shape;
  
  // * Definition *

  SgBasicBlock * kernel_body = kernel_defn->get_body();
  assert(kernel_body != NULL);

  // * Lookup parameter symbols *

  for (it_var_sym = arguments.parameters.begin(); it_var_sym != arguments.parameters.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("param_" + param_name);
    assert(arg_sym != NULL);

    local_symbol_maps.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, arg_sym));
  }

  // * Lookup scalar symbols *

  for (it_var_sym = arguments.scalars.begin(); it_var_sym != arguments.scalars.end(); it_var_sym++) {
    SgVariableSymbol * scalar_sym = *it_var_sym;
    std::string scalar_name = scalar_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("scalar_" + scalar_name);
    assert(arg_sym != NULL);

    local_symbol_maps.scalars.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(scalar_sym, arg_sym));
  }

  // * Lookup data symbols *

  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    ::KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();;
    std::string data_name = data_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("data_" + data_name);
    assert(arg_sym != NULL);

    local_symbol_maps.datas.insert(
      std::pair< ::KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *, SgVariableSymbol *>(data, arg_sym)
    );
  }

  // * Create iterator *

  for (it_loop_shape = loop_shapes.begin(); it_loop_shape != loop_shapes.end(); it_loop_shape++) {
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t * loop = it_loop_shape->first;
    ::KLT::Runtime::OpenACC::loop_shape_t * shape = it_loop_shape->second;

    SgVariableSymbol * iter_sym = loop->iterator;
    std::string iter_name = iter_sym->get_name().getString();
    SgType * iter_type = iter_sym->get_type();

    if (shape == NULL) {

      SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
        "local_it_" + iter_name, iter_type, NULL, kernel_body
      );
      SageInterface::appendStatement(iter_decl, kernel_body);

      SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_it_" + iter_name);
      assert(local_sym != NULL);
      local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
    }
    else {
      assert(loop->isDistributed());

      SgVariableSymbol * local_sym = NULL;

      if (shape->tile_0 != 1) {
        std::string name = "local_it_" + iter_name + "_tile_0";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        shape->iterators[0] = local_sym;
      }
      if (shape->gang != 1) {
        std::string name = "local_it_" + iter_name + "_gang";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        shape->iterators[1] = local_sym;
      }
      if (shape->tile_1 != 1) {
        std::string name = "local_it_" + iter_name + "_tile_1";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        shape->iterators[2] = local_sym;
      }
      if (shape->worker != 1) {
        std::string name = "local_it_" + iter_name + "_worker";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        shape->iterators[3] = local_sym;
      }
      if (shape->tile_2 != 1) {
        std::string name = "local_it_" + iter_name + "_tile_2";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        shape->iterators[4] = local_sym;
      }
      if (shape->vector == 0) {
        std::string name = "local_it_" + iter_name + "_vector";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        shape->iterators[5] = local_sym;
      }
      else if (shape->vector > 1) {
        assert(false); /// \todo no iterator with static vector length > 1, as vector expressions imply it
      }

      if (shape->tile_3 != 1) {
        std::string name = "local_it_" + iter_name + "_tile_3";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        shape->iterators[6] = local_sym;
      }

      assert(local_sym != NULL);

      local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));

    }
  }

  SgVariableSymbol * context_sym = kernel_defn->lookup_variable_symbol("context");
  assert(context_sym != NULL);

  local_symbol_maps.context = context_sym;

  return kernel_body;

}

}


