
#include "sage3basic.h"

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Language
typedef ::KLT::Language::C Klang; // Kernel Language

#include "MDCG/KLT/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"

#include "MFB/utils.hpp"

namespace MDCG {

namespace KLT {

namespace API {

SgExpression * buildLoopContextAccessor(size_t idx, SgVariableSymbol * ctx, SgFunctionSymbol * accessor) {
  return SageBuilder::buildFunctionCallExp(accessor, SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(ctx), SageBuilder::buildIntVal(idx)));
}

template <>
SgInitializedName * kernel_t<Klang>::createContext() const {
  return SageBuilder::buildInitializedName("context", SageBuilder::buildModifierType(SageBuilder::buildPointerType(klt_loop_context_class->get_declaration()->get_type())), NULL);
}

template <>
SgExpression * kernel_t<Klang>::buildLoopLower(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildLoopContextAccessor(loop_id, ctx, get_loop_lower_fnct);
}

template <>
SgExpression * kernel_t<Klang>::buildLoopUpper(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildLoopContextAccessor(loop_id, ctx, get_loop_upper_fnct);
}

template <>
SgExpression * kernel_t<Klang>::buildLoopStride(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildLoopContextAccessor(loop_id, ctx, get_loop_stride_fnct);
}

template <>
SgExpression * kernel_t<Klang>::buildTileLength(size_t tile_id, SgVariableSymbol * ctx) const {
  return buildLoopContextAccessor(tile_id, ctx, get_tile_length_fnct);
}

template <>
SgExpression * kernel_t<Klang>::buildTileStride(size_t tile_id, SgVariableSymbol * ctx) const {
  return buildLoopContextAccessor(tile_id, ctx, get_tile_stride_fnct);
}

template <>
void kernel_t<Klang>::load(const MDCG::Model::model_t & model) {
  klt_loop_context_class = model.lookup<MDCG::Model::class_t>("klt_loop_context_t")->node->symbol;
  assert(klt_loop_context_class != NULL);

  get_loop_lower_fnct = model.lookup<MDCG::Model::function_t>("klt_get_loop_lower")->node->symbol;
  assert(get_loop_lower_fnct != NULL);

  get_loop_upper_fnct = model.lookup<MDCG::Model::function_t>("klt_get_loop_upper")->node->symbol;
  assert(get_loop_upper_fnct != NULL);

  get_loop_stride_fnct = model.lookup<MDCG::Model::function_t>("klt_get_loop_stride")->node->symbol;
  assert(get_loop_stride_fnct != NULL);

  get_tile_length_fnct = model.lookup<MDCG::Model::function_t>("klt_get_tile_length")->node->symbol;
  assert(get_tile_length_fnct != NULL);

  get_tile_stride_fnct = model.lookup<MDCG::Model::function_t>("klt_get_tile_stride")->node->symbol;
  assert(get_tile_stride_fnct != NULL);
}

template <>
SgVariableSymbol * host_t<Hlang>::insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const {
  SgInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
                           build_kernel_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(kernel_id))
                         ));
  SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration("kernel", SageBuilder::buildPointerType(kernel_class->get_type()), init, scope);
  SageInterface::appendStatement(kernel_decl, scope);

  SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
  assert(kernel_sym != NULL);

  return kernel_sym;
}

template <>
void host_t<Hlang>::insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const {
  SageInterface::appendStatement(
    SageBuilder::buildFunctionCallStmt(
      SageBuilder::buildFunctionRefExp(execute_kernel_func),
      SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(kernel_sym))
    ), scope
  );
}

template <>
SgStatement * host_t<Hlang>::buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_param_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

template <>
SgStatement * host_t<Hlang>::buildScalarAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_scalar_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

template <>
SgStatement * host_t<Hlang>::buildDataAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

template <>
SgStatement * host_t<Hlang>::buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_lower_field), rhs));
}

template <>
SgStatement * host_t<Hlang>::buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_upper_field), rhs));
}

template <>
SgStatement * host_t<Hlang>::buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_stride_field), rhs));
}

template <>
void host_t<Hlang>::load(const MDCG::Model::model_t & model) {
  MDCG::Model::class_t kernel_class_ = model.lookup<MDCG::Model::class_t>("kernel_t");
  kernel_class = kernel_class_->node->symbol;
  assert(kernel_class != NULL);

    kernel_data_field   = kernel_class_->scope->field_children[1]->node->symbol;
    kernel_param_field  = kernel_class_->scope->field_children[2]->node->symbol;
    kernel_scalar_field = kernel_class_->scope->field_children[3]->node->symbol;
    kernel_loop_field   = kernel_class_->scope->field_children[4]->node->symbol;

  MDCG::Model::class_t loop_class_ = model.lookup<MDCG::Model::class_t>("klt_loop_t");
  loop_class = loop_class_->node->symbol;
  assert(loop_class != NULL);

    loop_lower_field  = loop_class_->scope->field_children[0]->node->symbol;
    loop_upper_field  = loop_class_->scope->field_children[1]->node->symbol;
    loop_stride_field = loop_class_->scope->field_children[2]->node->symbol;

  MDCG::Model::class_t tile_class_ = model.lookup<MDCG::Model::class_t>("klt_tile_t");
  tile_class = tile_class_->node->symbol;
  assert(tile_class != NULL);

  MDCG::Model::function_t build_kernel_func_ = model.lookup<MDCG::Model::function_t>("build_kernel");
  build_kernel_func = build_kernel_func_->node->symbol;
  assert(build_kernel_func != NULL);

  MDCG::Model::function_t execute_kernel_func_ = model.lookup<MDCG::Model::function_t>("execute_kernel");
  execute_kernel_func = execute_kernel_func_->node->symbol;
  assert(execute_kernel_func != NULL);

  MDCG::Model::type_t kernel_func_ptr_type_ = model.lookup<MDCG::Model::type_t>("kernel_func_ptr");
  kernel_func_ptr_type = kernel_func_ptr_type_->node->type;
  assert(kernel_func_ptr_type != NULL);

  load_user(model);
}

} // namespace MDCG::KLT::API

namespace Descriptor {

tile_t::tile_t() : id(0), kind((tile_kind_e)0), nbr_it(0), order(0), iterator_sym(NULL) {}

loop_t::loop_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_) : id(id_), lb(lb_), ub(ub_), stride(stride_) {}

} // namespace MDCG::KLT::Descriptor

template <> Runtime<Hlang, Klang>::host_api_t Runtime<Hlang, Klang>::host_api = Runtime<Hlang, Klang>::host_api_t();

template <> Runtime<Hlang, Klang>::kernel_api_t Runtime<Hlang, Klang>::kernel_api = Runtime<Hlang, Klang>::kernel_api_t();

template <> 
void Runtime<Hlang, Klang>::useSymbolsKernel(
  MFB::Driver<MFB::Sage> & driver,
  size_t file_id
) {
  driver.useSymbol<SgClassDeclaration>(kernel_api.getLoopContextClass(), file_id);
}

template <>
void Runtime<Hlang, Klang>::useSymbolsHost(
  MFB::Driver<MFB::Sage> & driver,
  size_t file_id
) {
  driver.useSymbol<SgClassDeclaration>(host_api.getTileClass(),   file_id);
  driver.useSymbol<SgClassDeclaration>(host_api.getLoopClass(),   file_id);
  driver.useSymbol<SgClassDeclaration>(host_api.getKernelClass(), file_id);
}

template <>
size_t Runtime<Hlang, Klang>::loadAPI(MDCG::ModelBuilder & model_builder, const std::string & KLT_RTL, const std::string & USER_RTL) {
  size_t tilek_model = model_builder.create();

  model_builder.add(tilek_model, "tile",    KLT_RTL, "h");
  model_builder.add(tilek_model, "loop",    KLT_RTL, "h");
  model_builder.add(tilek_model, "context", KLT_RTL, "h");

  model_builder.add(tilek_model, "kernel", USER_RTL, "h");

  host_api.load(model_builder.get(tilek_model));
  kernel_api.load(model_builder.get(tilek_model));

  return tilek_model;
}

} // namespace MDCG::KLT

} // namespace MDCG

