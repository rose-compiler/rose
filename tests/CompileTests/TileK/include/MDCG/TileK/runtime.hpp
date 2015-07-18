
#ifndef __MDCG_TILEK_RUNTIME_HPP__
#define __MDCG_TILEK_RUNTIME_HPP__

#include "MDCG/KLT/runtime.hpp"
#include "KLT/Language/c-family.hpp"

namespace MDCG {
namespace KLT {
namespace API {

template <>
struct host_t< ::KLT::Language::C>::user_t {
#ifdef TILEK_THREADS
  SgVariableSymbol * kernel_num_threads_field;
#endif
};

template <>
void host_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model);

} // namespace MDCG::KLT::API
} // namespace MDCG::KLT
} // namespace MDCG

#endif /* __MDCG_TILEK_RUNTIME_HPP__ */

