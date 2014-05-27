
#include "KLT/utils.hpp"

#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/iteration-mapper.hpp"
#include "KLT/Core/mfb-klt.hpp"

#include "KLT/OpenACC/language-opencl.hpp"
#include "KLT/OpenACC/runtime-openacc.hpp"
#include "KLT/OpenACC/dlx-openacc.hpp"
#include "KLT/OpenACC/mfb-acc-ocl.hpp"

#include "MFB/Sage/function-declaration.hpp"

#include "sage3basic.h"

namespace KLT {

template <>
unsigned long Generator<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC,
  MFB::KLT_Driver
>::createFile() {
  unsigned long file_id = p_sage_driver.add(boost::filesystem::path(p_file_name));
  p_sage_driver.attachArbitraryText(file_id, "#pragma OPENCL EXTENSION cl_khr_fp64: enable");
  return file_id;
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

  SgModifierType * context_type = SageBuilder::buildModifierType(
    SageBuilder::buildPointerType(
      Runtime::OpenACC::runtime_device_context_symbol->get_declaration()->get_type()
    )
  );
  context_type->get_typeModifier().setOpenclConstant();

  result->append_arg(SageBuilder::buildInitializedName("context", context_type, NULL));
  
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
        SgExpression * dim_size = SageInterface::copyExpression(data->getSections()[++cnt].second);
        subscript = SageBuilder::buildMultiplyOp(subscript, dim_size);
        subscript = SageBuilder::buildAddOp(subscript, SageInterface::copyExpression(*it_subscript));
      }

      SageInterface::replaceExpression(top_arr_ref, SageBuilder::buildPntrArrRefExp(SageInterface::copyExpression(var_ref), subscript), true);
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

    if (local_sym != NULL)
      SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym), true);
    else {
      /// \todo valid if sym can be found in local scope
    }
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

    SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym), true);
  }

  return result;
}

std::pair<SgStatement *, std::vector<SgScopeStatement *> > makeTile(
  SgVariableSymbol * tile_iterator,
  SgExpression * base,
  unsigned tile_id,
  SgExpression * runtime_loop_desc,
  long tile_length,
  bool unrolled,
  const Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>, Language::OpenCL, Runtime::OpenACC
  >::local_symbol_maps_t & local_symbol_maps
) {
  if (unrolled) {
    assert(tile_length > 1);
    SgBasicBlock * result_bb = SageBuilder::buildBasicBlock();
    std::vector<SgScopeStatement *> result_scopes;

    SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
                                    SageBuilder::buildVarRefExp(tile_iterator), base
                                   ));
    result_bb->append_statement(init_stmt);
    for (unsigned i = 0; i < tile_length; i++) {
      // Build an empty basic block where the iteration will be performed
      SgBasicBlock * bb = SageBuilder::buildBasicBlock();
      result_bb->append_statement(bb);
      result_scopes.push_back(bb);

      // Increment the iterator
      SgExprStatement * inc_stmt = SageBuilder::buildExprStatement(SageBuilder::buildPlusAssignOp(
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
                                   )); // 'tile_iterator' += 'runtime_loop_desc'.tiles['tile_id'].stride
      result_bb->append_statement(inc_stmt);
    }
    /// \todo add assert if language authorizes it: "assert('tile_iterator' == 'runtime_loop_desc'.tiles['tile_id'].upper)"
    return std::pair<SgStatement *, std::vector<SgScopeStatement *> >(result_bb, result_scopes);
  }
  else {
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
    SgExprStatement * test_stmt  = SageBuilder::buildExprStatement(SageBuilder::buildLessThanOp(SageBuilder::buildVarRefExp(tile_iterator), upper_bound));;
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

    return std::pair<SgStatement *, std::vector<SgScopeStatement *> >(for_stmt, std::vector<SgScopeStatement *>(1, for_body));
  }
  assert(false);
}

template <>
std::pair<SgStatement *, std::vector<SgScopeStatement *> > generateLoops<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC
> (
  LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t * loop,
  unsigned & loop_cnt,
  std::map<LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *, Runtime::OpenACC::a_loop> & loop_descriptors_map,
  Runtime::OpenACC::loop_shape_t * shape,
  const Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>, Language::OpenCL, Runtime::OpenACC
  >::local_symbol_maps_t & local_symbol_maps
) {
  if (!loop->isDistributed()) {
    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local = local_symbol_maps.iterators.find(loop->iterator);
    assert(it_sym_to_local != local_symbol_maps.iterators.end());
    SgVariableSymbol * local_it_sym = it_sym_to_local->second;

    SgExpression * lower_bound = translateConstExpression(loop->lower_bound, local_symbol_maps.parameters, local_symbol_maps.iterators);
    SgExpression * upper_bound = translateConstExpression(loop->upper_bound, local_symbol_maps.parameters, local_symbol_maps.iterators);

    SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(local_it_sym), lower_bound));
    SgExprStatement * test_stmt  = SageBuilder::buildExprStatement(SageBuilder::buildLessThanOp(SageBuilder::buildVarRefExp(local_it_sym), upper_bound));;
    SgExpression * inc_expr = SageBuilder::buildPlusAssignOp(
                                SageBuilder::buildVarRefExp(local_it_sym),
                                SageBuilder::buildIntVal(1) /// \todo add stride expression to LoopTrees' loops
                              );

    SgBasicBlock * for_body = SageBuilder::buildBasicBlock();
    SgForStatement * for_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, inc_expr, for_body);

    std::map<LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *, Runtime::OpenACC::a_loop>::const_iterator it_loop_desc = loop_descriptors_map.find(loop);
    if (it_loop_desc == loop_descriptors_map.end()) {
      Runtime::OpenACC::a_loop loop_desc;
        loop_desc.id = loop_cnt++;
        loop_desc.lb = loop->lower_bound;
        loop_desc.ub = loop->upper_bound;
        loop_desc.tile_0 = 0;
        loop_desc.gang   = 1;
        loop_desc.tile_1 = 1;
        loop_desc.worker = 1;
        loop_desc.tile_2 = 1;
        loop_desc.vector = 1;
        loop_desc.tile_3 = 1;
        loop_desc.unroll_tile_0 = false;
        loop_desc.unroll_tile_1 = false;
        loop_desc.unroll_tile_2 = false;
        loop_desc.unroll_tile_3 = false;

      loop_descriptors_map.insert(
          std::pair<
            LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *,
            Runtime::OpenACC::a_loop
          >(loop, loop_desc)
      );
    }

    return std::pair<SgStatement *, std::vector<SgScopeStatement *> >(for_stmt, std::vector<SgScopeStatement *>(1, for_body));
  }
  else {
    assert(shape != NULL);
    unsigned loop_id = 0;

    std::map<LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *, Runtime::OpenACC::a_loop>::const_iterator it_loop_desc = loop_descriptors_map.find(loop);
    if (it_loop_desc == loop_descriptors_map.end()) {
      Runtime::OpenACC::a_loop loop_desc;
        loop_desc.id = loop_cnt++;
        loop_desc.lb = loop->lower_bound;
        loop_desc.ub = loop->upper_bound;
        loop_desc.tile_0 = shape->tile_0;
        loop_desc.gang   = shape->gang;
        loop_desc.tile_1 = shape->tile_1;
        loop_desc.worker = shape->worker;
        loop_desc.tile_2 = shape->tile_2;
        loop_desc.vector = shape->vector;
        loop_desc.tile_3 = shape->tile_3;
        loop_desc.unroll_tile_0 = shape->unroll_tile_0;
        loop_desc.unroll_tile_1 = shape->unroll_tile_1;
        loop_desc.unroll_tile_2 = shape->unroll_tile_2;
        loop_desc.unroll_tile_3 = shape->unroll_tile_3;

      loop_id = loop_desc.id;

      loop_descriptors_map.insert(
          std::pair<
            LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *,
            Runtime::OpenACC::a_loop
          >(loop, loop_desc)
      );
    }
    else
      loop_id = it_loop_desc->second.id;

    std::vector<SgScopeStatement *>::iterator it_scopes;
    std::pair<SgStatement *, std::vector<SgScopeStatement *> > result(NULL, std::vector<SgScopeStatement *>(1, NULL));
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
                          ); // ctx->loops['loop_id'].original.lower

    if (shape->tile_0 != 1) {
      std::pair<SgStatement *, std::vector<SgScopeStatement *> > tile = makeTile(
        shape->iterators[0],
        SageInterface::copyExpression(base),
        0,
        SageInterface::copyExpression(runtime_loop_desc),
        shape->tile_0,
        shape->unroll_tile_0,
        local_symbol_maps
      );
      base = SageBuilder::buildVarRefExp(shape->iterators[0]);

      if (result.first == NULL) {
        result.first = tile.first;
        result.second.clear();
        result.second.insert(result.second.end(), tile.second.begin(), tile.second.end());
      }
      else
        assert(false); // Not reachable: first tile
    }

    if (shape->gang != 1) {
      for (it_scopes = result.second.begin(); it_scopes != result.second.end(); it_scopes++) {
        SgStatement * set_gang_it = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
          SageBuilder::buildVarRefExp(shape->iterators[1]),
          SageBuilder::buildFunctionCallExp(
            SageBuilder::buildFunctionRefExp(Runtime::OpenACC::runtime_device_function_symbols.gang_iter_symbol),
            SageBuilder::buildExprListExp(
              SageBuilder::buildVarRefExp(local_symbol_maps.context),
              SageBuilder::buildIntVal(loop_id),
              SageInterface::copyExpression(base)
            )
          )
        ));
        if (*it_scopes == NULL) {
          assert(result.first == NULL && result.second.size() == 1);
          *it_scopes = SageBuilder::buildBasicBlock();
          result.first = *it_scopes;
        }
        SageInterface::appendStatement(set_gang_it, *it_scopes);
      }
      base = SageBuilder::buildVarRefExp(shape->iterators[1]);
    }

    if (shape->tile_1 != 1) {
      std::vector<SgScopeStatement *> scopes;
      scopes.reserve(result.second.size());
      for (it_scopes = result.second.begin(); it_scopes != result.second.end(); it_scopes++) {
        std::pair<SgStatement *, std::vector<SgScopeStatement *> > tile = makeTile(
          shape->iterators[2], SageInterface::copyExpression(base), 2, SageInterface::copyExpression(runtime_loop_desc), shape->tile_1, shape->unroll_tile_1, local_symbol_maps
        );

        if (*it_scopes == NULL) {
          assert(result.first == NULL && result.second.size() == 1);
          result.first = tile.first;
          scopes.insert(scopes.end(), tile.second.begin(), tile.second.end());
        }
        else {
          assert(result.first != NULL);

          SageInterface::appendStatement(tile.first, *it_scopes);
          scopes.insert(scopes.end(), tile.second.begin(), tile.second.end());
        }
      }
      result.second = scopes;
      base = SageBuilder::buildVarRefExp(shape->iterators[2]);
    }

    if (shape->worker != 1) {
      for (it_scopes = result.second.begin(); it_scopes != result.second.end(); it_scopes++) {
        SgStatement * set_worker_it = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
          SageBuilder::buildVarRefExp(shape->iterators[3]),
          SageBuilder::buildFunctionCallExp(
            SageBuilder::buildFunctionRefExp(Runtime::OpenACC::runtime_device_function_symbols.worker_iter_symbol),
            SageBuilder::buildExprListExp(
              SageBuilder::buildVarRefExp(local_symbol_maps.context),
              SageBuilder::buildIntVal(loop_id),
              SageInterface::copyExpression(base)
            )
          )
        ));
        if (*it_scopes == NULL) {
          assert(result.first == NULL && result.second.size() == 1);
          *it_scopes = SageBuilder::buildBasicBlock();
          result.first = *it_scopes;
        }
        SageInterface::appendStatement(set_worker_it, *it_scopes);
      }
      base = SageBuilder::buildVarRefExp(shape->iterators[3]);
    }

    if (shape->tile_2 != 1) {
      std::vector<SgScopeStatement *> scopes;
      scopes.reserve(result.second.size());
      for (it_scopes = result.second.begin(); it_scopes != result.second.end(); it_scopes++) {
        std::pair<SgStatement *, std::vector<SgScopeStatement *> > tile = makeTile(
          shape->iterators[4], SageInterface::copyExpression(base), 4, SageInterface::copyExpression(runtime_loop_desc), shape->tile_2, shape->unroll_tile_2, local_symbol_maps
        );

        if (*it_scopes == NULL) {
          assert(result.first == NULL && result.second.size() == 1);
          result.first = tile.first;
          scopes.insert(scopes.end(), tile.second.begin(), tile.second.end());
        }
        else {
          assert(result.first != NULL);

          SageInterface::appendStatement(tile.first, *it_scopes);
          scopes.insert(scopes.end(), tile.second.begin(), tile.second.end());
        }
      }
      result.second = scopes;
      base = SageBuilder::buildVarRefExp(shape->iterators[4]);
    }

    if (shape->vector == 0) {
      assert(false); // Dynamic vector cannot be generated
    }
    else if (shape->vector > 1) {
      assert(false); /// \todo generate vector expression from contained statement, should be done by 'generateStatement'
    }

    if (shape->tile_3 != 1) {
      std::vector<SgScopeStatement *> scopes;
      scopes.reserve(result.second.size());
      for (it_scopes = result.second.begin(); it_scopes != result.second.end(); it_scopes++) {
        std::pair<SgStatement *, std::vector<SgScopeStatement *> > tile = makeTile(
          shape->iterators[6], SageInterface::copyExpression(base), 6, SageInterface::copyExpression(runtime_loop_desc), shape->tile_3, shape->unroll_tile_3, local_symbol_maps
        );

        if (*it_scopes == NULL) {
          assert(result.first == NULL && result.second.size() == 1);
          result.first = tile.first;
          scopes.insert(scopes.end(), tile.second.begin(), tile.second.end());
        }
        else {
          assert(result.first != NULL);

          SageInterface::appendStatement(tile.first, *it_scopes);
          scopes.insert(scopes.end(), tile.second.begin(), tile.second.end());
        }
      }
      result.second = scopes;
      base = SageBuilder::buildVarRefExp(shape->iterators[6]);
    }

    return result;
  }

  assert(false);
}

}

