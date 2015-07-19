
#include "sage3basic.h"

#include "KLT/Language/c-family.hpp"

#include "MDCG/KLT/runtime.hpp"

namespace MDCG {

namespace KLT {

template <> Runtime< ::KLT::Language::C, ::KLT::Language::C>::host_api_t Runtime< ::KLT::Language::C, ::KLT::Language::C>::host_api = Runtime< ::KLT::Language::C, ::KLT::Language::C>::host_api_t();
template <> Runtime< ::KLT::Language::C, ::KLT::Language::C>::kernel_api_t Runtime< ::KLT::Language::C, ::KLT::Language::C>::kernel_api = Runtime< ::KLT::Language::C, ::KLT::Language::C>::kernel_api_t();

template <> Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::host_api_t Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::host_api = Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::host_api_t();
template <> Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::kernel_api_t Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::kernel_api = Runtime< ::KLT::Language::C, ::KLT::Language::OpenCL>::kernel_api_t();

template <> Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::host_api_t Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::host_api = Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::host_api_t();
template <> Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::kernel_api_t Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::kernel_api = Runtime< ::KLT::Language::C, ::KLT::Language::CUDA>::kernel_api_t();

} // namespace MDCG::KLT

} // namespace MDCG

