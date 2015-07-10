
#include "sage3basic.h"

#include "KLT/Runtime/tilek.hpp"
#include "KLT/Language/none.hpp"

#include "KLT/Core/runtime.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"

namespace KLT {

namespace Runtime {

TileK::tile_desc_t::tile_desc_t(DLX::TileK::language_t::tile_clause_t * tile_clause) :
  id(0), kind((tile_kind_e)tile_clause->parameters.kind), nbr_it(tile_clause->parameters.nbr_it), order(tile_clause->parameters.order), iterator_sym(NULL)
{}

TileK::loop_desc_t::loop_desc_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_) :
  id(id_), lb(lb_), ub(ub_), stride(stride_)
{}

TileK::kernel_api_t TileK::kernel_api;

// Build: 'expr'->'array'['idx'] or 'expr'->'array'['idx'].'field'
SgExpression * buildPtrArrElemField(SgExpression * expr, SgVariableSymbol * array, SgExpression * idx, SgVariableSymbol * field = NULL) {
  SgExpression * result = SageBuilder::buildPntrArrRefExp(SageBuilder::buildArrowExp(expr, SageBuilder::buildVarRefExp(array)), idx);
  if (field != NULL) result = SageBuilder::buildDotExp(result, SageBuilder::buildVarRefExp(field));
  return result;
}

// Build: 'ctx'->loops['loop_id'].lower
SgExpression * TileK::kernel_api_t::buildLoopLower(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_loop_field, SageBuilder::buildIntVal(loop_id), context_loop_lower_field);
}

// Build: 'ctx'->loops['loop_id'].upper
SgExpression * TileK::kernel_api_t::buildLoopUpper(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_loop_field, SageBuilder::buildIntVal(loop_id), context_loop_upper_field);
}

// Build: 'ctx'->loops['loop_id'].stride
SgExpression * TileK::kernel_api_t::buildLoopStride(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_loop_field, SageBuilder::buildIntVal(loop_id), context_loop_stride_field);
}

// Build: 'ctx'->tiles['tile_id'].length
SgExpression * TileK::kernel_api_t::buildTileLength(size_t tile_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_tile_field, SageBuilder::buildIntVal(tile_id), context_tile_length_field);
}

// Build: 'ctx'->tiles['tile_id'].stride
SgExpression * TileK::kernel_api_t::buildTileStride(size_t tile_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_tile_field, SageBuilder::buildIntVal(tile_id), context_tile_stride_field);
}

TileK::host_api_t TileK::host_api;

// Insert: "struct kernel_t * kernel = build_kernel('kernel->id');"
SgVariableSymbol * TileK::host_api_t::insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const {
  SgInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
                           build_kernel_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(kernel_id))
                         ));
  SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration("kernel", SageBuilder::buildPointerType(kernel_class->get_type()), init, scope);
  SageInterface::appendStatement(kernel_decl, scope);

  SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
  assert(kernel_sym != NULL);

  return kernel_sym;
}

// Insert: "execute_kernel(kernel);"
void TileK::host_api_t::insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const {
  SageInterface::appendStatement(
    SageBuilder::buildFunctionCallStmt(
      SageBuilder::buildFunctionRefExp(execute_kernel_func),
      SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(kernel_sym))
    ), scope
  );
}

// Build: 'kernel'->params['idx'] = 'rhs'
SgStatement * TileK::host_api_t::buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_param_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

// Build: 'kernel'->datas['idx'] = 'rhs'
SgStatement * TileK::host_api_t::buildDataAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

// Build: 'kernel'->scalars['idx'] = 'rhs'
SgStatement * TileK::host_api_t::buildScalarAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_scalar_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

// Build: 'kernel'->loops['idx'].lower = 'rhs'
SgStatement * TileK::host_api_t::buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_lower_field), rhs));
}

// Build: 'kernel'->loops['idx'].upper = 'rhs'
SgStatement * TileK::host_api_t::buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_upper_field), rhs));
}

// Build: 'kernel'->loops['idx'].stride = 'rhs'
SgStatement * TileK::host_api_t::buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_stride_field), rhs));
}


unsigned TileK::loadAPI(MDCG::ModelBuilder & model_builder, const std::string & headers_path) {
  unsigned tilek_model = model_builder.create();
    model_builder.add(tilek_model, "tile",   headers_path, "h");
    model_builder.add(tilek_model, "loop",   headers_path, "h");
    model_builder.add(tilek_model, "kernel", headers_path, "h");
  loadAPI(model_builder.get(tilek_model));
  return tilek_model;
}

void TileK::loadAPI(const MDCG::Model::model_t & model) {

  MDCG::Model::class_t context_class = model.lookup<MDCG::Model::class_t>("context_t");
  kernel_api.context_class = context_class->node->symbol;
  assert(kernel_api.context_class != NULL);

    kernel_api.context_loop_field = context_class->scope->field_children[0]->node->symbol;
    kernel_api.context_tile_field = context_class->scope->field_children[1]->node->symbol;

  MDCG::Model::class_t kernel_class = model.lookup<MDCG::Model::class_t>("kernel_t");
  host_api.kernel_class = kernel_class->node->symbol;
  assert(host_api.kernel_class != NULL);

    host_api.kernel_data_field  = kernel_class->scope->field_children[1]->node->symbol;
    host_api.kernel_param_field = kernel_class->scope->field_children[2]->node->symbol;
    host_api.kernel_scalar_field = kernel_class->scope->field_children[3]->node->symbol;
    host_api.kernel_loop_field  = kernel_class->scope->field_children[4]->node->symbol;

  MDCG::Model::class_t loop_class = model.lookup<MDCG::Model::class_t>("loop_t");
  kernel_api.context_loop_class = loop_class->node->symbol;
  assert(kernel_api.context_loop_class != NULL);

    kernel_api.context_loop_lower_field  = loop_class->scope->field_children[0]->node->symbol;
    kernel_api.context_loop_upper_field  = loop_class->scope->field_children[1]->node->symbol;
    kernel_api.context_loop_stride_field = loop_class->scope->field_children[2]->node->symbol;

    host_api.loop_lower_field  = loop_class->scope->field_children[0]->node->symbol;
    host_api.loop_upper_field  = loop_class->scope->field_children[1]->node->symbol;
    host_api.loop_stride_field = loop_class->scope->field_children[2]->node->symbol;

  MDCG::Model::class_t tile_class = model.lookup<MDCG::Model::class_t>("tile_t");
  kernel_api.context_tile_class = tile_class->node->symbol;
  assert(kernel_api.context_tile_class != NULL);

    kernel_api.context_tile_length_field = tile_class->scope->field_children[0]->node->symbol;
    kernel_api.context_tile_stride_field = tile_class->scope->field_children[1]->node->symbol;

  MDCG::Model::function_t build_kernel_func = model.lookup<MDCG::Model::function_t>("build_kernel");
  host_api.build_kernel_func = build_kernel_func->node->symbol;
  assert(host_api.build_kernel_func != NULL);

  MDCG::Model::function_t execute_kernel_func = model.lookup<MDCG::Model::function_t>("execute_kernel");
  host_api.execute_kernel_func = execute_kernel_func->node->symbol;
  assert(host_api.execute_kernel_func != NULL);

  MDCG::Model::type_t kernel_func_ptr_type = model.lookup<MDCG::Model::type_t>("kernel_func_ptr");
  host_api.kernel_func_ptr_type = kernel_func_ptr_type->node->type;
  assert(host_api.kernel_func_ptr_type != NULL);
}

void TileK::useSymbolsKernel(
  MFB::Driver<MFB::Sage> & driver,
  unsigned long file_id
) {
  driver.useSymbol<SgClassDeclaration>(kernel_api.context_tile_class, file_id);
  driver.useSymbol<SgClassDeclaration>(kernel_api.context_loop_class, file_id);
  driver.useSymbol<SgClassDeclaration>(kernel_api.context_class, file_id);
}

void TileK::useSymbolsHost(
  MFB::Driver<MFB::Sage> & driver,
  unsigned long file_id
) {
  driver.useSymbol<SgClassDeclaration>(host_api.kernel_class, file_id);
  driver.useSymbol<SgClassDeclaration>(kernel_api.context_tile_class, file_id);
  driver.useSymbol<SgClassDeclaration>(kernel_api.context_loop_class, file_id);
}

} // namespace KLT::Runtime

} // namespace KLT

