
#ifndef __MDCG_TILEK_RUNTIME_HPP__
#define __MDCG_TILEK_RUNTIME_HPP__

#include "MDCG/KLT/runtime.hpp"
#include "KLT/Language/c-family.hpp"

namespace MDCG {
namespace KLT {

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL);
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::applyKernelModifiers(SgFunctionDeclaration * kernel_decl);
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::addRuntimeStaticData(MFB::Driver<MFB::Sage> & driver, const std::string & KLT_RTL, const std::string & USER_RTL, const std::string & kernel_file_name, const std::string & static_file_name, size_t static_file_id);
#  elif defined(TILEK_TARGET_CUDA)
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL);
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::applyKernelModifiers(SgFunctionDeclaration * kernel_decl);
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::addRuntimeStaticData(MFB::Driver<MFB::Sage> & driver, const std::string & KLT_RTL, const std::string & USER_RTL, const std::string & kernel_file_name, const std::string & static_file_name, size_t static_file_id);
#  endif
#else
template <> void Runtime< ::KLT::Language::C, ::KLT::Language::C>::loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL);
#endif

namespace API {

template <>
struct host_t< ::KLT::Language::C>::user_t {
#if !defined(TILEK_ACCELERATOR)
  SgType * kernel_func_ptr_type;
#endif
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
  SgFunctionSymbol * opencl_get_group_id_func;
  SgFunctionSymbol * opencl_get_local_id_func;

  SgExpression * buildGetGangID(size_t lvl) const;
  SgExpression * buildGetWorkerID(size_t lvl) const;
};

template <>
void kernel_t< ::KLT::Language::OpenCL>::load_user(const MDCG::Model::model_t & model);
#  elif defined(TILEK_TARGET_CUDA)
template <>
struct kernel_t< ::KLT::Language::CUDA>::user_t {
  SgVariableSymbol * cuda_threadIdx_var;
  SgVariableSymbol * cuda_blockIdx_var;
  SgVariableSymbol * cuda_uint3_fields[3];

  SgExpression * buildGetGangID(size_t lvl) const;
  SgExpression * buildGetWorkerID(size_t lvl) const;
};

template <>
void kernel_t< ::KLT::Language::CUDA>::load_user(const MDCG::Model::model_t & model);
#  endif
#else
template <>
struct kernel_t< ::KLT::Language::C>::user_t {};
template <>
void kernel_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model);
template <>
SgType * kernel_t< ::KLT::Language::OpenCL>::addContextTypeModifier(SgType * type) const;
#endif

} // namespace MDCG::KLT::API
} // namespace MDCG::KLT
} // namespace MDCG

#endif /* __MDCG_TILEK_RUNTIME_HPP__ */

