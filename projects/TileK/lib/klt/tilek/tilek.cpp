
#include "KLT/utils.hpp"

#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-tiler.hpp"
#include "KLT/Core/mfb-klt.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/function-declaration.hpp"

#include "KLT/TileK/tilek.hpp"

#include "MDCG/model.hpp"
#include "MDCG/model-class.hpp"

#include "sage3basic.h"

namespace KLT {

template <>
unsigned long Generator<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK,
  MFB::KLT_Driver
>::createFile() {
  unsigned long file_id = p_sage_driver.create(boost::filesystem::path(p_file_name));
  return file_id;
}

template <>
SgFunctionParameterList * createParameterList<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
>(
  Kernel<
    DLX::KLT_Annotation<DLX::TileK::language_t>,
    Language::None,
    Runtime::TileK
  > * kernel
) {
  const std::list<SgVariableSymbol *> & params = kernel->getArguments().parameters;
  const std::list<SgVariableSymbol *> & scalars = kernel->getArguments().scalars;
  const std::list<Data<DLX::KLT_Annotation<DLX::TileK::language_t> > *> & datas = kernel->getArguments().datas;
  unsigned long data_type_modifer_ = SgTypeModifier::e_ocl_global__;

  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list<Data<DLX::KLT_Annotation<DLX::TileK::language_t> > *>::const_iterator it_data;

  SgFunctionParameterList * result = SageBuilder::buildFunctionParameterList();

  result->append_arg(SageBuilder::buildInitializedName("param", SageBuilder::buildPointerType(SageBuilder::buildIntType()), NULL));	
  result->append_arg(SageBuilder::buildInitializedName("data",  SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));

  assert(Runtime::TileK::tilek_kernel_api.context_class != NULL);
  result->append_arg(SageBuilder::buildInitializedName(
    "context", SageBuilder::buildModifierType(SageBuilder::buildPointerType(Runtime::TileK::tilek_kernel_api.context_class->get_declaration()->get_type())), NULL
  ));

  return result;
}

template <>
SgStatement * generateStatement<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
> (
  LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::stmt_t * stmt,
  const Kernel<
    DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK
  >::local_symbol_maps_t & local_symbol_maps
) {
  SgStatement * result = SageInterface::copyStatement(stmt->statement);

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;
  std::map<Data<DLX::KLT_Annotation<DLX::TileK::language_t> > *, SgVariableSymbol *>::const_iterator it_data_to_local;

  std::map<SgVariableSymbol *, SgVariableSymbol *> data_sym_to_local;
  std::map<SgVariableSymbol *, Data<DLX::KLT_Annotation<DLX::TileK::language_t> > *> data_sym_to_data;

  for (it_data_to_local = local_symbol_maps.datas.begin(); it_data_to_local != local_symbol_maps.datas.end(); it_data_to_local++) {
    Data<DLX::KLT_Annotation<DLX::TileK::language_t> > * data = it_data_to_local->first;
    SgVariableSymbol * data_sym = it_data_to_local->first->getVariableSymbol();
    SgVariableSymbol * local_sym = it_data_to_local->second;

    data_sym_to_local.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, local_sym));
    data_sym_to_data.insert(std::pair<SgVariableSymbol *, Data<DLX::KLT_Annotation<DLX::TileK::language_t> > *>(data_sym, data));
  }

  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();

    std::map<SgVariableSymbol *, Data<DLX::KLT_Annotation<DLX::TileK::language_t> > *>::const_iterator it_data_sym_to_data = data_sym_to_data.find(var_sym);
    if (it_data_sym_to_data == data_sym_to_data.end()) continue; // Not a variable reference to a Data

    Data<DLX::KLT_Annotation<DLX::TileK::language_t> > * data = it_data_sym_to_data->second;

    SgPntrArrRefExp * arr_ref = isSgPntrArrRefExp(var_ref->get_parent());
    SgPntrArrRefExp * top_arr_ref = NULL;
    std::vector<SgExpression *> subscripts;
    while (arr_ref != NULL) {
      top_arr_ref = arr_ref;
      subscripts.push_back(arr_ref->get_rhs_operand_i());
      arr_ref = isSgPntrArrRefExp(arr_ref->get_parent());
    }
    assert(subscripts.size() == data->getSections().size());

    if (data->getSections().size() > 1) {
      assert(top_arr_ref != NULL);
      std::vector<SgExpression *>::const_iterator it_subscript = subscripts.begin();
      SgExpression * subscript = SageInterface::copyExpression(*it_subscript);
      it_subscript++;
      size_t cnt = 1;
      while (it_subscript != subscripts.end()) {
        SgExpression * dim_size = SageInterface::copyExpression(data->getSections()[cnt].size);
        subscript = SageBuilder::buildMultiplyOp(subscript, dim_size);
        subscript = SageBuilder::buildAddOp(subscript, SageInterface::copyExpression(*it_subscript));
        cnt++; it_subscript++;
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

template <>
std::pair<SgStatement *, SgScopeStatement *> generateLoops<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
> (
  LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t * loop,
  const Kernel<
    DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK
  >::local_symbol_maps_t & local_symbol_maps
) {
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local = local_symbol_maps.iterators.find(loop->iterator);
  assert(it_sym_to_local != local_symbol_maps.iterators.end());

  SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
                                  SageBuilder::buildVarRefExp(it_sym_to_local->second),
                                  translateConstExpression(loop->lower_bound, local_symbol_maps.iterators, local_symbol_maps.parameters)
                                ));
  SgExprStatement * test_stmt  = SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(
                                   SageBuilder::buildVarRefExp(it_sym_to_local->second),
                                   translateConstExpression(loop->upper_bound, local_symbol_maps.iterators, local_symbol_maps.parameters))
                                 );
  SgExpression * inc_expr = SageBuilder::buildPlusAssignOp(
                              SageBuilder::buildVarRefExp(it_sym_to_local->second),
                              translateConstExpression(loop->stride, local_symbol_maps.iterators, local_symbol_maps.parameters)
                            );
  SgBasicBlock * for_body = SageBuilder::buildBasicBlock();
  SgForStatement * for_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, inc_expr, for_body);

  return std::pair<SgStatement *, SgScopeStatement *>(for_stmt, for_body);
}

template <>
tile_generation_t<DLX::KLT_Annotation<DLX::TileK::language_t> > generateTiles<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
> (
  LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::tile_t * tile,
  const Kernel<
    DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK
  >::local_symbol_maps_t & local_symbol_maps
) {
  std::map<LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t *, SgVariableSymbol *> loop_iterator_map;
  SgForStatement * first_for_stmt = NULL;
  SgForStatement * last_for_stmt = NULL;
  LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::block_t * block = NULL;
  while (tile != NULL) {
    LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t * loop = tile->loop;
    assert(loop != NULL);

    SgVariableSymbol * tile_iterator = tile->iterator_sym;
    assert(tile_iterator != NULL);

    std::map<LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t *, SgVariableSymbol *>::iterator it_loop_iterator = loop_iterator_map.find(loop);
    SgVariableSymbol * previous_iterator = NULL;
    if (it_loop_iterator != loop_iterator_map.end()) {
      previous_iterator = it_loop_iterator->second;
      it_loop_iterator->second = tile_iterator;
    }
    else {
      loop_iterator_map.insert(std::pair<LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t *, SgVariableSymbol *>(loop, tile_iterator));
    }

    SgExpression * lower_bound = NULL;
    SgExpression * upper_bound = NULL;
    if (previous_iterator == NULL) {
      lower_bound = translateConstExpression(loop->lower_bound, local_symbol_maps.iterators, local_symbol_maps.parameters);
      upper_bound = translateConstExpression(loop->upper_bound, local_symbol_maps.iterators, local_symbol_maps.parameters);
    }
    else {
      lower_bound = SageBuilder::buildVarRefExp(previous_iterator);
      upper_bound = SageBuilder::buildAddOp(
                      SageBuilder::buildVarRefExp(previous_iterator),
                      SageBuilder::buildDotExp(
                        SageBuilder::buildPntrArrRefExp(
                          SageBuilder::buildArrowExp(
                            SageBuilder::buildVarRefExp(local_symbol_maps.context),
                            SageBuilder::buildVarRefExp(Runtime::TileK::tilek_kernel_api.context_tile_field)
                          ),
                          SageBuilder::buildIntVal(tile->id)
                        ),
                        SageBuilder::buildVarRefExp(Runtime::TileK::tilek_kernel_api.context_tile_length_field)
                      )
                    ); // 'tile_iterator' + 'ctx'->tile['tile_id'].length
    }

    SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(tile_iterator), lower_bound));
    SgExprStatement * test_stmt = SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(SageBuilder::buildVarRefExp(tile_iterator), upper_bound));
    SgExpression * inc_expr = SageBuilder::buildPlusAssignOp(
                                SageBuilder::buildVarRefExp(tile_iterator),
                                SageBuilder::buildDotExp(
                                  SageBuilder::buildPntrArrRefExp(
                                    SageBuilder::buildArrowExp(
                                      SageBuilder::buildVarRefExp(local_symbol_maps.context),
                                      SageBuilder::buildVarRefExp(Runtime::TileK::tilek_kernel_api.context_tile_field)
                                    ),
                                    SageBuilder::buildIntVal(tile->id)
                                  ),
                                  SageBuilder::buildVarRefExp(Runtime::TileK::tilek_kernel_api.context_tile_stride_field)
                                )
                              ); // 'tile_iterator' += 'ctx'->tile['tile_id'].stride

    SgForStatement * for_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, inc_expr, NULL);

    if (last_for_stmt != NULL) {
      SageInterface::setLoopBody(last_for_stmt, for_stmt);
    }
    else
      first_for_stmt = for_stmt;
    last_for_stmt = for_stmt;

    block = tile->block;
    tile = tile->tile;
  }
  assert(block != NULL);

  SgBasicBlock * body = SageBuilder::buildBasicBlock();
  SageInterface::setLoopBody(last_for_stmt, body);

  std::map<LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t *, SgVariableSymbol *>::const_iterator it_loop_iterator;
  for (it_loop_iterator = loop_iterator_map.begin(); it_loop_iterator != loop_iterator_map.end(); it_loop_iterator++) {
    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local = local_symbol_maps.iterators.find(it_loop_iterator->first->iterator);
    assert(it_sym_to_local != local_symbol_maps.iterators.end());
    SageInterface::appendStatement(SageBuilder::buildAssignStatement(
      SageBuilder::buildVarRefExp(it_sym_to_local->second), SageBuilder::buildVarRefExp(it_loop_iterator->second)
    ), body);
  }

  return tile_generation_t<DLX::KLT_Annotation<DLX::TileK::language_t> >(first_for_stmt, body, block);
}

template <>
Runtime::TileK::exec_mode_t changeExecutionMode<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
> (
  const Runtime::TileK::exec_mode_t & exec_mode,
  const Runtime::TileK::exec_config_t & exec_cfg
) {
  return exec_mode;
}

template <>
void generateSynchronizations<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
> (
  Runtime::TileK::exec_mode_t prev_exec_mode,
  Runtime::TileK::exec_mode_t next_exec_mode,
  const Runtime::TileK::exec_config_t & exec_cfg,
  SgScopeStatement * scope,
  const Kernel<
    DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK
  >::local_symbol_maps_t & local_symbol_maps
) {}

template <>
SgScopeStatement * generateExecModeGuards<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
> (
  Runtime::TileK::exec_mode_t exec_mode,
  const Runtime::TileK::exec_config_t & exec_cfg,
  SgScopeStatement * scope,
  const Kernel<
    DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK
  >::local_symbol_maps_t & local_symbol_maps
) {
  return scope;
}

namespace Runtime {

TileK::tilek_kernel_api_t TileK::tilek_kernel_api;
TileK::tilek_host_api_t TileK::tilek_host_api;

void TileK::loadAPI(const MDCG::Model::model_t & model) {

  MDCG::Model::class_t context_class = model.lookup<MDCG::Model::class_t>("context_t");
  tilek_kernel_api.context_class = context_class->node->symbol;
  assert(tilek_kernel_api.context_class != NULL);

    tilek_kernel_api.context_loop_field = context_class->scope->field_children[0]->node->symbol;
    tilek_kernel_api.context_tile_field = context_class->scope->field_children[1]->node->symbol;

  MDCG::Model::class_t loop_class = model.lookup<MDCG::Model::class_t>("loop_t");
  tilek_kernel_api.context_loop_class = loop_class->node->symbol;
  assert(tilek_kernel_api.context_loop_class != NULL);

    tilek_kernel_api.context_loop_lower_field  = loop_class->scope->field_children[0]->node->symbol;
    tilek_kernel_api.context_loop_upper_field  = loop_class->scope->field_children[1]->node->symbol;
    tilek_kernel_api.context_loop_stride_field = loop_class->scope->field_children[2]->node->symbol;

  MDCG::Model::class_t tile_class = model.lookup<MDCG::Model::class_t>("tile_t");
  tilek_kernel_api.context_tile_class = tile_class->node->symbol;
  assert(tilek_kernel_api.context_tile_class != NULL);

    tilek_kernel_api.context_tile_length_field = tile_class->scope->field_children[0]->node->symbol;
    tilek_kernel_api.context_tile_stride_field = tile_class->scope->field_children[1]->node->symbol;

  // TODO Host API
}

void TileK::useSymbolsKernel(
  MFB::Driver<MFB::Sage> & driver,
  unsigned long file_id
) {
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::tilek_kernel_api.context_tile_class, file_id);
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::tilek_kernel_api.context_loop_class, file_id);
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::tilek_kernel_api.context_class, file_id);
}

void TileK::useSymbolsHost(
  MFB::Driver<MFB::Sage> & driver,
  unsigned long file_id
) {
  // TODO
}

template <>
void get_exec_config<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
> (
  TileK::exec_config_t & exec_config,
  const Kernel<DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK> * kernel
) {}

}

template <>
LoopTiler<DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::loop_tiling_t::loop_tiling_t(
  LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t * loop_
) :
  loop(loop_),
  tiles()
{
  std::vector<DLX::KLT_Annotation<DLX::TileK::language_t> >::const_iterator it_annotation;
  for (it_annotation = loop->annotations.begin(); it_annotation != loop->annotations.end(); it_annotation++) {
    if (
      it_annotation->clause->kind == DLX::TileK::language_t::e_clause_tile
    ) {
      Runtime::TileK::tile_desc_t & tile_desc = *(tiles.insert(tiles.end(), Runtime::TileK::tile_desc_t()));
      switch (it_annotation->clause->kind) {
        case DLX::TileK::language_t::e_clause_tile:
        {
          switch (((DLX::Directives::clause_t<DLX::TileK::language_t, DLX::TileK::language_t::e_clause_tile> *)(it_annotation->clause))->parameters.kind) {
            case DLX::Directives::generic_clause_t<DLX::TileK::language_t>::parameters_t<DLX::TileK::language_t::e_clause_tile>::e_static_tile:
              tile_desc.kind = Runtime::TileK::e_static_tile;
              tile_desc.nbr_it = ((DLX::Directives::clause_t<DLX::TileK::language_t, DLX::TileK::language_t::e_clause_tile> *)(it_annotation->clause))->parameters.nbr_it;
              break;
            case DLX::Directives::generic_clause_t<DLX::TileK::language_t>::parameters_t<DLX::TileK::language_t::e_clause_tile>::e_dynamic_tile:
              tile_desc.kind = Runtime::TileK::e_dynamic_tile;
              tile_desc.nbr_it = 0;
              break;
            default:
              assert(false);
          }
          tile_desc.order = ((DLX::Directives::clause_t<DLX::TileK::language_t, DLX::TileK::language_t::e_clause_tile> *)(it_annotation->clause))->parameters.order;
          break;
        }
      }
    }
  }
}

template <>
size_t LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::id_cnt = 0;

template <>
bool LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t::isDistributed() const {
  return false;
}

template <>
bool LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t::isSplitted() const {
  return false;
}

template <>
void printAnnotations<DLX::KLT_Annotation<DLX::TileK::language_t> >(
  const std::vector<DLX::KLT_Annotation<DLX::TileK::language_t> > & annotations,
  std::ostream & out,
  std::string indent
) {
  out << DLX::TileK::language_t::language_label << "(";
  if (!annotations.empty()) { 
    std::vector<DLX::KLT_Annotation<DLX::TileK::language_t> >::const_iterator it_annotation = annotations.begin();
    out << it_annotation->clause->kind;
    it_annotation++;
    for (; it_annotation != annotations.end(); it_annotation++) {
      out << ", ";
      out << it_annotation->clause->kind;
    }
  }
  out << "), " << std::endl;
}

template <>
unsigned long Kernel<DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::id_cnt = 0;

template <>
unsigned long Kernel<DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::kernel_desc_t::id_cnt = 0;

template <>
bool Data<DLX::KLT_Annotation<DLX::TileK::language_t> >::isFlowIn() const {
  return true;
}

template <>
bool Data<DLX::KLT_Annotation<DLX::TileK::language_t> >::isFlowOut() const {
  return true;
}

template <>
void DataFlow<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
>::markSplittedData(
  const context_t & context
) const {}

namespace Language {

void None::applyKernelModifiers(SgFunctionDeclaration * kernel_decl) {}

}

}

namespace DLX {

template <>
void KLT_Annotation<TileK::language_t>::parseRegion(std::vector<DLX::KLT_Annotation<TileK::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<TileK::language_t> & annotation = container.back();
}

template <>
void KLT_Annotation<TileK::language_t>::parseData(std::vector<DLX::KLT_Annotation<TileK::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<TileK::language_t> & annotation = container.back();
}

template <>
void KLT_Annotation<TileK::language_t>::parseLoop(std::vector<DLX::KLT_Annotation<TileK::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<TileK::language_t> & annotation = container.back();

  switch (annotation.clause->kind) {
    case TileK::language_t::e_clause_tile:
      /// \todo 'lvl' and expression parameter
      break;
    default:
      assert(false);
  }
}

}

namespace MFB {

KLT<tilek_kernel_t>::object_desc_t::object_desc_t(
  unsigned id_,
  tilek_kernel_t * kernel_,
  unsigned long file_id_
) :
  id(id_),
  kernel(kernel_),
  file_id(file_id_),
  tiling()
{}

SgVariableSymbol * getExistingSymbolOrBuildDecl(
  const std::string & name,
  SgType * type,
  SgScopeStatement * scope
) {
  SgVariableSymbol * sym = scope->lookup_variable_symbol(name);
  if (sym == NULL) {
    SgVariableDeclaration * decl = SageBuilder::buildVariableDeclaration(name, type, NULL, scope);
    SageInterface::appendStatement(decl, scope);
    sym = scope->lookup_variable_symbol(name);
  }
  assert(sym != NULL);
  return sym;
}

template <>
SgBasicBlock * createLocalDeclarations<
  DLX::KLT_Annotation<DLX::TileK::language_t>,
  ::KLT::Language::None,
  ::KLT::Runtime::TileK
>(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  ::KLT::Kernel<
    DLX::KLT_Annotation<DLX::TileK::language_t>,
    ::KLT::Language::None,
    ::KLT::Runtime::TileK
  >::local_symbol_maps_t & local_symbol_maps,
  const ::KLT::Kernel<
    DLX::KLT_Annotation<DLX::TileK::language_t>,
    ::KLT::Language::None,
    ::KLT::Runtime::TileK
  >::arguments_t & arguments,
  const std::map<
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t *,
    ::KLT::LoopTiler<DLX::KLT_Annotation<DLX::TileK::language_t>, ::KLT::Language::None, ::KLT::Runtime::TileK>::loop_tiling_t *
  > & loop_tiling
) {
  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Data<DLX::KLT_Annotation<DLX::TileK::language_t> > *>::const_iterator it_data;

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_param_to_field;
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_scalar_to_field;
  std::map< ::KLT::Data<DLX::KLT_Annotation<DLX::TileK::language_t> > *, SgVariableSymbol *>::const_iterator it_data_to_field;

  std::map<
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t *,
    ::KLT::LoopTiler<DLX::KLT_Annotation<DLX::TileK::language_t>, ::KLT::Language::None, ::KLT::Runtime::TileK>::loop_tiling_t *
  >::const_iterator it_loop_tiling;
  
  // * Definition *

  SgBasicBlock * kernel_body = kernel_defn->get_body();
  assert(kernel_body != NULL);

  // * Lookup parameter symbols *

  SgVariableSymbol * arg_param_sym = kernel_defn->lookup_variable_symbol("param");
  assert(arg_param_sym != NULL);

  int arg_cnt = 0;
  for (it_var_sym = arguments.parameters.begin(); it_var_sym != arguments.parameters.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();

    SageInterface::prependStatement(
      SageBuilder::buildVariableDeclaration(
        param_name,
        param_sym->get_type(),
        SageBuilder::buildAssignInitializer(SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_param_sym), SageBuilder::buildIntVal(arg_cnt))),
        kernel_body
      ),
      kernel_body
    );
    SgVariableSymbol * new_sym = kernel_body->lookup_variable_symbol(param_name);
    assert(new_sym != NULL);

    local_symbol_maps.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, new_sym));
    arg_cnt++;
  }

  // * Lookup data symbols *

  SgVariableSymbol * arg_data_sym = kernel_defn->lookup_variable_symbol("data");
  assert(arg_data_sym != NULL);

  arg_cnt = 0;
  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    ::KLT::Data<DLX::KLT_Annotation<DLX::TileK::language_t> > * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();
    std::string data_name = data_sym->get_name().getString();

    SgExpression * init;
    if (data->getSections().size() > 0)
      init = SageBuilder::buildCastExp(
        SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_data_sym), SageBuilder::buildIntVal(arg_cnt)),
        data_sym->get_type()
      );
    else
      init = SageBuilder::buildPointerDerefExp(SageBuilder::buildCastExp(
        SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_data_sym), SageBuilder::buildIntVal(arg_cnt)),
        SageBuilder::buildPointerType(data_sym->get_type())
      ));

    SageInterface::prependStatement(
      SageBuilder::buildVariableDeclaration(
        data_name,
        data_sym->get_type(),
        SageBuilder::buildAssignInitializer(init),
        kernel_body
      ),
      kernel_body
    );
    SgVariableSymbol * new_sym = kernel_body->lookup_variable_symbol(data_name);
    assert(new_sym != NULL);

    local_symbol_maps.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, new_sym));
    arg_cnt++;
  }

  // * Create iterator *

  for (it_loop_tiling = loop_tiling.begin(); it_loop_tiling != loop_tiling.end(); it_loop_tiling++) {
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t * loop = it_loop_tiling->first;
    ::KLT::LoopTiler<DLX::KLT_Annotation<DLX::TileK::language_t>, ::KLT::Language::None, ::KLT::Runtime::TileK>::loop_tiling_t * tiling = it_loop_tiling->second;

    SgVariableSymbol * iter_sym = loop->iterator;
    std::string iter_name = iter_sym->get_name().getString();
    SgType * iter_type = iter_sym->get_type();

    std::ostringstream oss_loop;
    oss_loop << "it_" << loop->id;
    SgVariableSymbol * local_sym = getExistingSymbolOrBuildDecl(oss_loop.str(), iter_type, kernel_body);
    local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));

    size_t tile_cnt = 0;
    std::vector< ::KLT::Runtime::TileK::tile_desc_t>::iterator it_tile;
    for (it_tile = tiling->tiles.begin(); it_tile != tiling->tiles.end(); it_tile++) {
      std::ostringstream oss_tile;
      oss_tile << "it_" << loop->id << "_" << tile_cnt++;
      it_tile->iterator_sym = getExistingSymbolOrBuildDecl(oss_tile.str(), iter_type, kernel_body);
    }
  }

  local_symbol_maps.context = kernel_defn->lookup_variable_symbol("context");
  assert(local_symbol_maps.context != NULL);

  return kernel_body;

}

}

