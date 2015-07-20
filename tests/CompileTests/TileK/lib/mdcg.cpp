
#include "sage3basic.h"

#include "MDCG/TileK/runtime.hpp"

#include <cassert>

namespace MDCG {
namespace KLT {

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
template <>
void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL) {
  model_builder.add(tilek_model, "kernel", USER_RTL + "/Host/", "h");
  model_builder.add(tilek_model, "kernel", USER_RTL + "/Kernel/OpenCL/", "cl");
}
#  elif defined(TILEK_TARGET_CUDA)
template <>
void Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL) {
  model_builder.add(tilek_model, "kernel", USER_RTL + "/Host/", "h");
  model_builder.add(tilek_model, "kernel", USER_RTL + "/Kernel/CUDA/", "cu");
}
#  endif
#else
template <>
void Runtime< ::KLT::Language::C, ::KLT::Language::C>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL) {
  model_builder.add(tilek_model, "kernel", USER_RTL + "/Host/", "h");
}
#endif

namespace API {

template <>
void host_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model) {
  assert(user == NULL);
  user = new user_t();
#if defined(TILEK_THREADS)
  MDCG::Model::class_t kernel_class = model.lookup<MDCG::Model::class_t>("kernel_t");
  assert(kernel_class->scope->field_children.size() == 6);
  user->kernel_num_threads_field = kernel_class->scope->field_children[5]->node->symbol;
#elif defined(TILEK_ACCELERATOR)
  MDCG::Model::class_t kernel_class = model.lookup<MDCG::Model::class_t>("kernel_t");
  assert(kernel_class->scope->field_children.size() == 7);
  user->kernel_num_gangs_field   = kernel_class->scope->field_children[5]->node->symbol;
  user->kernel_num_workers_field = kernel_class->scope->field_children[6]->node->symbol;
#endif
}

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
template <>
void kernel_t< ::KLT::Language::OpenCL>::load_user(const MDCG::Model::model_t & model) {
  assert(false); // TODO
}
#  elif defined(TILEK_TARGET_CUDA)
template <>
void kernel_t< ::KLT::Language::CUDA>::load_user(const MDCG::Model::model_t & model) {
  assert(false); // TODO
}
#  endif
#else
template <>
void kernel_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model) {}
#endif

} // namespace MDCG::KLT::API
} // namespace MDCG::KLT
} // namespace MDCG

