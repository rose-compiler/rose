
#ifndef __MDCG_TILEK_MODEL_HPP__
#define __MDCG_TILEK_MODEL_HPP__

#include "MDCG/KLT/model.hpp"

#include "KLT/Core/kernel.hpp"

namespace MDCG {

namespace TileK {

template <class Annotation_, class Runtime_>
struct KernelDesc {
  typedef ::KLT::Kernel<Annotation_, Runtime_> Kernel;
  typedef Kernel * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    assert(input->getKernels().size() == 1);
    typename Kernel::kernel_desc_t * kernel = input->getKernels()[0];
    assert(kernel != NULL);

    switch (field_id) {
      case 0:
      { // struct klt_data_container_t data;
        Model::class_t field_class = StaticInitializer::getBaseClass(element, "data", "klt_data_container_t");
        return static_initializer.createInitializer< ::MDCG::KLT::DataContainer<Annotation_, Runtime_> >(field_class, input, file_id);
      }
      case 1:
      { // struct klt_loop_container_t loop;
        Model::class_t field_class = StaticInitializer::getBaseClass(element, "loop", "klt_loop_container_t");
        return static_initializer.createInitializer< ::MDCG::KLT::LoopContainer<Annotation_, Runtime_> >(field_class, input, file_id);
      }
#if defined(TILEK_BASIC) || defined(TILEK_THREADS)
      case 2:
      { // kernel_func_ptr kernel_ptr;
        MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(kernel->kernel_name, Runtime::host_api.user->kernel_func_ptr_type, NULL, NULL, file_id, false, true);
        MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = static_initializer.getDriver().build<SgVariableDeclaration>(var_decl_desc);

        SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(var_decl_res.symbol->get_declaration()->get_parent());
          decl_stmt->get_declarationModifier().unsetDefault();
          decl_stmt->get_declarationModifier().get_storageModifier().setExtern();

        return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(var_decl_res.symbol));
      }
#elif defined(TILEK_ACCELERATOR)
      case 2:
      { // char * kernel_name;
        return SageBuilder::buildStringVal(kernel->kernel_name);
      }
#endif
      default:
        assert(false);
    }
  }
};

} // namespace MDCG::TileK

} // namespace MDCG

#endif /* __MDCG_TILEK_MODEL_HPP__ */

