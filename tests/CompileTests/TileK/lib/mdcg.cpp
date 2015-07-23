
#include "sage3basic.h"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include "MDCG/TileK/runtime.hpp"

#include <cassert>

namespace MDCG {
namespace KLT {

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
template <>
void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL) {
  model_builder.add(tilek_model, "kernel", USER_RTL + "/include/RTL/Host", "h");
  model_builder.add(tilek_model, "kernel", USER_RTL + "/include/RTL/Kernel/OpenCL", "cl"); // Empty, used to get OpenCL's built-in
}
template <>
void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::applyKernelModifiers(SgFunctionDeclaration * kernel_decl) {
  kernel_decl->get_functionModifier().setOpenclKernel();
}
template <>
void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::addRuntimeStaticData(
  MFB::Driver<MFB::Sage> & driver, const std::string & KLT_RTL, const std::string & USER_RTL, const std::string & kernel_file_name, const std::string & static_file_name, size_t static_file_id
) {
  driver.build<SgVariableDeclaration>(
    MFB::Sage<SgVariableDeclaration>::object_desc_t(
      "opencl_kernel_file",
      SageBuilder::buildPointerType(SageBuilder::buildCharType()),
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal(kernel_file_name)),
      NULL, static_file_id, false, true
    )
  );
  driver.build<SgVariableDeclaration>(
    MFB::Sage<SgVariableDeclaration>::object_desc_t(
      "opencl_kernel_options",
      SageBuilder::buildPointerType(SageBuilder::buildCharType()),
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal("-I" + KLT_RTL + "/include -I" + USER_RTL + "/include")),
      NULL, static_file_id, false, true
    )
  );
  driver.build<SgVariableDeclaration>(
    MFB::Sage<SgVariableDeclaration>::object_desc_t(
      "opencl_klt_runtime_lib",
      SageBuilder::buildPointerType(SageBuilder::buildCharType()),
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal(KLT_RTL + "/lib/rtl/context.c")),
      NULL, static_file_id, false, true
    )
  );
}
#  elif defined(TILEK_TARGET_CUDA)
template <>
void Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL) {
  model_builder.add(tilek_model, "kernel", USER_RTL + "/include/RTL/Host", "h");
  model_builder.add(tilek_model, "kernel", USER_RTL + "/include/RTL/Kernel/CUDA", "cu"); // Empty, used to get CUDA's built-in
}
#  endif
#else
template <>
void Runtime< ::KLT::Language::C, ::KLT::Language::C>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL) {
  model_builder.add(tilek_model, "kernel", USER_RTL + "/include/RTL/Host", "h");
}
#endif

namespace API {

template <>
void host_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model) {
  assert(user == NULL);
  user = new user_t();
#if defined(TILEK_THREADS)
  MDCG::Model::class_t kernel_class = model.lookup<MDCG::Model::class_t>("kernel_t");
  assert(kernel_class->scope->field_children.size() == 7);
  user->kernel_num_threads_field = kernel_class->scope->field_children[6]->node->symbol;
#elif defined(TILEK_ACCELERATOR)
  MDCG::Model::class_t kernel_class = model.lookup<MDCG::Model::class_t>("kernel_t");
  assert(kernel_class->scope->field_children.size() == 8);
  user->kernel_num_gangs_field   = kernel_class->scope->field_children[6]->node->symbol;
  user->kernel_num_workers_field = kernel_class->scope->field_children[7]->node->symbol;
#endif
}

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
template <>
void kernel_t< ::KLT::Language::OpenCL>::load_user(const MDCG::Model::model_t & model) {
  assert(user == NULL);
  user = new user_t();

  user->opencl_get_group_id_func = model.lookup<MDCG::Model::function_t>("get_group_id")->node->symbol;
  assert(user->opencl_get_group_id_func != NULL);

  user->opencl_get_local_id_func = model.lookup<MDCG::Model::function_t>("get_local_id")->node->symbol;
  assert(user->opencl_get_local_id_func != NULL);
}

template <>
SgType * kernel_t< ::KLT::Language::OpenCL>::addContextTypeModifier(SgType * type) const {
  SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclGlobal();
  return mod_type;
}

SgExpression * kernel_t< ::KLT::Language::OpenCL>::user_t::buildGetGangID(size_t lvl) const {
  return SageBuilder::buildFunctionCallExp(opencl_get_group_id_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(lvl)));
}

SgExpression * kernel_t< ::KLT::Language::OpenCL>::user_t::buildGetWorkerID(size_t lvl) const {
  return SageBuilder::buildFunctionCallExp(opencl_get_local_id_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(lvl)));
}
#  elif defined(TILEK_TARGET_CUDA)
template <>
void kernel_t< ::KLT::Language::CUDA>::load_user(const MDCG::Model::model_t & model) {
  assert(false); // TODO
}

SgExpression * kernel_t< ::KLT::Language::CUDA>::user_t::buildGetGangID(size_t lvl) const {
  assert(false); // TODO
//return SageBuilder::buildFunctionCallExp(opencl_get_group_id_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(lvl)));
}

SgExpression * kernel_t< ::KLT::Language::CUDA>::user_t::buildGetWorkerID(size_t lvl) const {
  assert(false); // TODO
//return SageBuilder::buildFunctionCallExp(opencl_get_local_id_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(lvl)));
}
#  endif
#else
template <>
void kernel_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model) {}
#endif

} // namespace MDCG::KLT::API
} // namespace MDCG::KLT
} // namespace MDCG

