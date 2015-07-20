
#ifndef __MDCG_TILEK_RUNTIME_HPP__
#define __MDCG_TILEK_RUNTIME_HPP__

#include "MDCG/KLT/runtime.hpp"
#include "KLT/Language/c-family.hpp"

namespace MDCG {
namespace KLT {

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL);
#  elif defined(TILEK_TARGET_CUDA)
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL);
#  endif
#else
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::C>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL);
#endif

namespace API {

template <>
struct host_t< ::KLT::Language::C>::user_t {
#if defined(TILEK_THREADS)
  SgVariableSymbol * kernel_num_threads_field;
#elif defined(TILEK_ACCELERATOR)
  SgVariableSymbol * kernel_num_gangs_field;
  SgVariableSymbol * kernel_num_workers_field;
#endif
};

template <>
void host_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model);

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
template <>
struct kernel_t< ::KLT::Language::OpenCL>::user_t {
  SgExpression * buildGetGangID(size_t id, SgVariableSymbol * ctx) const;
  SgExpression * buildGetWorkerID(size_t id, SgVariableSymbol * ctx) const;
};

template <>
void kernel_t< ::KLT::Language::OpenCL>::load_user(const MDCG::Model::model_t & model);
#  elif defined(TILEK_TARGET_CUDA)
template <>
struct kernel_t< ::KLT::Language::CUDA>::user_t {
  SgExpression * buildGetGangID(size_t id, SgVariableSymbol * ctx) const;
  SgExpression * buildGetWorkerID(size_t id, SgVariableSymbol * ctx) const;
};

template <>
void kernel_t< ::KLT::Language::CUDA>::load_user(const MDCG::Model::model_t & model);
#  endif
#else
template <>
struct kernel_t< ::KLT::Language::C>::user_t {};

template <>
void kernel_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model);
#endif

} // namespace MDCG::KLT::API
} // namespace MDCG::KLT
} // namespace MDCG

#endif /* __MDCG_TILEK_RUNTIME_HPP__ */

