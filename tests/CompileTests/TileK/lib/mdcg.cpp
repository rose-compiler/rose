
#include "sage3basic.h"

#include "MDCG/TileK/runtime.hpp"

namespace MDCG {
namespace KLT {
namespace API {

template <>
void host_t< ::KLT::Language::C>::load_user(const MDCG::Model::model_t & model) {
  assert(user == NULL);
  user = new user_t();
#ifdef TILEK_THREADS
  MDCG::Model::class_t kernel_class = model.lookup<MDCG::Model::class_t>("kernel_t");
  assert(kernel_class->scope->field_children.size() == 6);
  user->kernel_num_threads_field = kernel_class->scope->field_children[5]->node->symbol;
  assert(user->kernel_num_threads_field != NULL);
  assert(user->kernel_num_threads_field->get_declaration() != NULL);
#endif
}

} // namespace MDCG::KLT::API
} // namespace MDCG::KLT
} // namespace MDCG

