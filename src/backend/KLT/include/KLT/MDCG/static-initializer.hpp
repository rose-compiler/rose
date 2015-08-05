
#include "MDCG/Tools/static-initializer.hpp"
#include "KLT/utils.hpp"

namespace KLT {

namespace MDCG {

template <class language_tpl>
struct KernelContainer {
  typedef std::map<typename language_tpl::directive_t *, Utils::subkernel_result_t<language_tpl> > input_t;

  static SgExpression * createFieldInitializer(
    const ::MDCG::Tools::StaticInitializer & static_initializer,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      default:
        assert(false);
    }
  }
};

}

}

