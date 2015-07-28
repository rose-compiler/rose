
#include "sage3basic.h"

#include "MFB/utils.hpp"

#include "MDCG/Core/model.hpp"
#include "MDCG/Core/model-function.hpp"
#include "MDCG/Core/model-class.hpp"

#include "KLT/api.hpp"

namespace KLT {

namespace API {

kernel_t::kernel_t() :
  klt_loop_context_class(NULL),
  get_loop_lower_fnct(NULL), get_loop_upper_fnct(NULL), get_loop_stride_fnct(NULL),
  get_tile_length_fnct(NULL), get_tile_stride_fnct(NULL)
{}

kernel_t::~kernel_t() {}

SgType * kernel_t::addContextTypeModifier(SgType * type) const { return type; }

SgInitializedName * kernel_t::createContext() const {
  return SageBuilder::buildInitializedName("context", addContextTypeModifier(SageBuilder::buildPointerType(klt_loop_context_class->get_declaration()->get_type())), NULL);
}

SgExpression * kernel_t::buildGetLoopLower (size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_lower_fnct);
}
SgExpression * kernel_t::buildGetLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_upper_fnct);
}
SgExpression * kernel_t::buildGetLoopStride(size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_stride_fnct);
}

SgExpression * kernel_t::buildGetTileLength(size_t tile_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(tile_id, ctx, get_tile_length_fnct);
}
SgExpression * kernel_t::buildGetTileStride(size_t tile_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(tile_id, ctx, get_tile_stride_fnct);
}

SgClassSymbol * kernel_t::getLoopContextClass() const { return klt_loop_context_class; }

void kernel_t::load(const MDCG::Model::model_t & model) {
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

  load_user(model);
}

void kernel_t::load_user(const MDCG::Model::model_t & model) {}

host_t::host_t() :
  kernel_class(NULL), loop_class(NULL), tile_class(NULL),
  kernel_param_field(NULL), kernel_data_field(NULL),
  kernel_loop_field(NULL), loop_lower_field(NULL), loop_upper_field(NULL), loop_stride_field(NULL),
  build_kernel_func(NULL), execute_kernel_func(NULL)
{}

host_t::~host_t() {}

SgVariableSymbol * host_t::insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const {
  SgInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
                           build_kernel_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(kernel_id))
                         ));
  SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration("kernel", SageBuilder::buildPointerType(kernel_class->get_type()), init, scope);
  SageInterface::appendStatement(kernel_decl, scope);

  SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
  assert(kernel_sym != NULL);

  return kernel_sym;
}

void host_t::insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const {
  SageInterface::appendStatement(
    SageBuilder::buildFunctionCallStmt(
      SageBuilder::buildFunctionRefExp(execute_kernel_func),
      SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(kernel_sym))
    ), scope
  );
}

SgStatement * host_t::buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_param_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

SgStatement * host_t::buildDataPtrAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_ptr_field), rhs));
}

SgStatement * host_t::buildDataSectionOffsetAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
           MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_section_field), SageBuilder::buildIntVal(dim)
         ), SageBuilder::buildVarRefExp(section_offset_field)), rhs));
}

SgStatement * host_t::buildDataSectionLengthAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
           MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_section_field), SageBuilder::buildIntVal(dim)
         ), SageBuilder::buildVarRefExp(section_length_field)), rhs));
}

SgStatement * host_t::buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_lower_field), rhs));
}

SgStatement * host_t::buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_upper_field), rhs));
}

SgStatement * host_t::buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_stride_field), rhs));
}

SgClassSymbol * host_t::getKernelClass() const { return kernel_class; }
SgClassSymbol * host_t::getLoopClass() const { return loop_class; }
SgClassSymbol * host_t::getTileClass() const { return tile_class; }
SgClassSymbol * host_t::getDataClass() const { return data_class; }

void host_t::load(const MDCG::Model::model_t & model) {
  MDCG::Model::class_t kernel_class_ = model.lookup<MDCG::Model::class_t>("kernel_t");
  kernel_class = kernel_class_->node->symbol;
  assert(kernel_class != NULL);

    kernel_param_field  = kernel_class_->scope->getField("param" )->node->symbol;
    kernel_data_field   = kernel_class_->scope->getField("data"  )->node->symbol;
    kernel_loop_field   = kernel_class_->scope->getField("loops" )->node->symbol;

  MDCG::Model::class_t data_class_ = model.lookup<MDCG::Model::class_t>("klt_data_t");
  data_class = data_class_->node->symbol;
  assert(data_class != NULL);

    data_ptr_field     = data_class_->scope->getField("ptr"     )->node->symbol;
    data_section_field = data_class_->scope->getField("sections")->node->symbol;

  MDCG::Model::class_t data_section_class = model.lookup<MDCG::Model::class_t>("klt_data_section_t");

    section_offset_field = data_section_class->scope->getField("offset")->node->symbol;
    section_length_field = data_section_class->scope->getField("length")->node->symbol;

  MDCG::Model::class_t loop_class_ = model.lookup<MDCG::Model::class_t>("klt_loop_t");
  loop_class = loop_class_->node->symbol;
  assert(loop_class != NULL);

    loop_lower_field  = loop_class_->scope->getField("lower" )->node->symbol;
    loop_upper_field  = loop_class_->scope->getField("upper" )->node->symbol;
    loop_stride_field = loop_class_->scope->getField("stride")->node->symbol;

  MDCG::Model::class_t tile_class_ = model.lookup<MDCG::Model::class_t>("klt_tile_t");
  tile_class = tile_class_->node->symbol;
  assert(tile_class != NULL);

  MDCG::Model::function_t build_kernel_func_ = model.lookup<MDCG::Model::function_t>("build_kernel");
  build_kernel_func = build_kernel_func_->node->symbol;
  assert(build_kernel_func != NULL);

  MDCG::Model::function_t execute_kernel_func_ = model.lookup<MDCG::Model::function_t>("execute_kernel");
  execute_kernel_func = execute_kernel_func_->node->symbol;
  assert(execute_kernel_func != NULL);

  load_user(model);
}

void host_t::load_user(const MDCG::Model::model_t & model) {}

} // namespace KLT::API

} // namespace KLT

