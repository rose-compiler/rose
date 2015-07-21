
#ifndef __MDCG_TILEK_MODEL_HPP__
#define __MDCG_TILEK_MODEL_HPP__

#include "MDCG/KLT/model.hpp"

#include "KLT/Core/kernel.hpp"
typedef ::KLT::Kernel<Annotation, Runtime> Kernel; // KLT's kernel object

namespace MDCG {

namespace TileK {

template <class Hlang, class Klang>
struct KernelDesc {
  typedef Kernel * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  );
};

template <>
SgExpression * KernelDesc<Hlang, Klang>::createFieldInitializer(
  const MDCG::StaticInitializer & static_initializer,
  MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  assert(input->getKernels().size() == 1);
  Kernel::kernel_desc_t * kernel = input->getKernels()[0];
  assert(kernel != NULL);

  switch (field_id) {
    case 0:
    { // int num_param;
      return SageBuilder::buildIntVal(input->getArguments().parameters.size());
    }
    case 1:
    { // int num_scalar;
      return SageBuilder::buildIntVal(input->getArguments().scalars.size());
    }
    case 2:
    { // int num_data;
      return SageBuilder::buildIntVal(input->getArguments().datas.size());
    }
    case 3:
    { // int num_priv;
      return SageBuilder::buildIntVal(input->getArguments().privates.size());
    }
    case 4:
    { // int num_loops;
      return SageBuilder::buildIntVal(kernel->loops.size());
    }
    case 5:
    { // int num_tiles;
      return SageBuilder::buildIntVal(kernel->tiles.size());
    }
    case 6:
    { // struct loop_desc_t * loop_desc;
      std::ostringstream decl_name; decl_name << "loop_" << kernel;
      MDCG::Model::class_t field_class = StaticInitializer::getBaseClassForPointerOnClass(element, "loop_desc", "klt_loop_desc_t");
      return static_initializer.createArrayPointer< ::MDCG::KLT::LoopDesc<Hlang> >(field_class, kernel->loops.size(), kernel->loops.begin(), kernel->loops.end(), file_id, decl_name.str());
    }
#if !defined(TILEK_ACCELERATOR) || !defined(TILEK_TARGET_OPENCL)
    case 7:
    { // kernel_func_ptr kernel_ptr;
      MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(kernel->kernel_name, Runtime::host_api.getKernelFnctPtrType(), NULL, NULL, file_id, false, true);
      MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = static_initializer.getDriver().build<SgVariableDeclaration>(var_decl_desc);

      SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(var_decl_res.symbol->get_declaration()->get_parent());
        decl_stmt->get_declarationModifier().unsetDefault();
        decl_stmt->get_declarationModifier().get_storageModifier().setExtern();

      return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(var_decl_res.symbol));
    }
#else
    case 7:
    { // char * kernel_name;
      return SageBuilder::buildStringVal(kernel->kernel_name);
    }
#endif
    default:
      assert(false);
  }
}

} // namespace MDCG::TileK

} // namespace MDCG

#endif /* __MDCG_TILEK_MODEL_HPP__ */

