
#include "sage3basic.h"

#include "KLT/TileK/api-opencl.hpp"

#if !defined(VERBOSE)
#  define VERBOSE 0
#endif

namespace KLT {

namespace TileK {

SgInitializedName * opencl_kernel_t::buildGlobalVariable(const std::string & name, SgType * type, SgInitializer * init) const {
  SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclGlobal();
  return SageBuilder::buildInitializedName(name, mod_type, init);
}

SgInitializedName * opencl_kernel_t::buildLocalVariable(const std::string & name, SgType * type, SgInitializer * init) const {
  SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclLocal();
  return SageBuilder::buildInitializedName(name, mod_type, init);
}

SgInitializedName * opencl_kernel_t::buildConstantVariable(const std::string & name, SgType * type, SgInitializer * init) const {
  SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclConstant();
  return SageBuilder::buildInitializedName(name, mod_type, init);
}

void opencl_kernel_t::applyKernelModifiers(SgFunctionDeclaration * kernel_decl) const {
  kernel_decl->get_functionModifier().setOpenclKernel();
}

void opencl_kernel_t::loadUser(const ::MDCG::Model::model_t & model) {
  assert(SageInterface::getProject() != NULL);
  assert(SageInterface::getProject()->get_globalScopeAcrossFiles() != NULL);
  get_group_id_func = SageInterface::getProject()->get_globalScopeAcrossFiles()->lookup_function_symbol("get_group_id"); assert(get_group_id_func != NULL);
  get_local_id_func = SageInterface::getProject()->get_globalScopeAcrossFiles()->lookup_function_symbol("get_local_id"); assert(get_local_id_func != NULL);
}

SgExpression * opencl_kernel_t::buildGangsTileIdx(size_t lvl) const {
  return SageBuilder::buildFunctionCallExp(get_group_id_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(lvl)));
}

SgExpression * opencl_kernel_t::buildWorkersTileIdx(size_t lvl) const {
  return SageBuilder::buildFunctionCallExp(get_local_id_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(lvl)));
}

} // namespace KLT::TileK

} // namespace KLT

