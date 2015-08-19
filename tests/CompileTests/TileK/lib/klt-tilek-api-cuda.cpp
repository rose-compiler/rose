
#include "sage3basic.h"

#include "KLT/TileK/api-cuda.hpp"

#if !defined(VERBOSE)
#  define VERBOSE 0
#endif

namespace KLT {

namespace TileK {

SgInitializedName * cuda_kernel_t::buildGlobalVariable(const std::string & name, SgType * type, SgInitializer * init) const {
  SgInitializedName * res = SageBuilder::buildInitializedName(name, type, init);
    res->get_storageModifier().setCudaGlobal();
  return res;
}

SgInitializedName * cuda_kernel_t::buildLocalVariable(const std::string & name, SgType * type, SgInitializer * init) const {
  SgInitializedName * res = SageBuilder::buildInitializedName(name, type, init);
    res->get_storageModifier().setCudaShared();
  return res;
}

SgInitializedName * cuda_kernel_t::buildConstantVariable(const std::string & name, SgType * type, SgInitializer * init) const {
  SgInitializedName * res = SageBuilder::buildInitializedName(name, type, init);
    res->get_storageModifier().setCudaConstant();
  return res;
}

void cuda_kernel_t::applyKernelModifiers(SgFunctionDeclaration * kernel_decl) const {
  kernel_decl->get_functionModifier().setOpenclKernel();
}

void cuda_kernel_t::loadUser(const ::MDCG::Model::model_t & model) {
  bool res = true;

  ::MDCG::Model::variable_t var_;
    res = api_t::load(var_ , thread_idx_var, model, "threadIdx" , NULL); assert(res == true);
    res = api_t::load(var_ ,  block_idx_var, model,  "blockIdx" , NULL); assert(res == true);

  ::MDCG::Model::class_t class_;
  SgClassSymbol * class_sym;
  ::MDCG::Model::field_t fld_;
    res = api_t::load(class_ , class_sym, model, "uint3" , NULL); assert(res == true);
    res = api_t::load(fld_ , uint3_field[0], model, "x" , class_); assert(res == true);
    res = api_t::load(fld_ , uint3_field[1], model, "y" , class_); assert(res == true);
    res = api_t::load(fld_ , uint3_field[2], model, "z" , class_); assert(res == true);
}

SgExpression * cuda_kernel_t::buildGangsTileIdx(size_t lvl) const {
  return SageBuilder::buildDotExp(SageBuilder::buildVarRefExp(block_idx_var), SageBuilder::buildVarRefExp(uint3_field[lvl]));
}

SgExpression * cuda_kernel_t::buildWorkersTileIdx(size_t lvl) const {
  return SageBuilder::buildDotExp(SageBuilder::buildVarRefExp(thread_idx_var), SageBuilder::buildVarRefExp(uint3_field[lvl]));
}

} // namespace KLT::TileK

} // namespace KLT

