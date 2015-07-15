
#ifndef __MDCG_KLT_MODEL_HPP__
#define __MDCG_KLT_MODEL_HPP__

#include "DLX/TileK/language.hpp"
typedef ::DLX::TileK::language_t Dlang; // Directives' Language

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Code Language
typedef ::KLT::Language::C Klang; // Kernel Language

#include "DLX/KLT/annotations.hpp"
typedef ::DLX::KLT::Annotation<Dlang> Annotation; // Alias for clauses parsed by DLX

#include "MDCG/KLT/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "KLT/Core/kernel.hpp"
typedef ::KLT::Kernel<Annotation, Runtime> Kernel; // KLT's kernel object

#include "MDCG/Core/static-initializer.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

namespace MDCG {

namespace KLT {

template <class Hlang>
struct TileDesc {
  typedef MDCG::KLT::Descriptor::tile_t * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  );
};

template <class Hlang>
struct LoopDesc {
  typedef MDCG::KLT::Descriptor::loop_t * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  );
};

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
SgExpression * TileDesc<Hlang>::createFieldInitializer(
  const MDCG::StaticInitializer & static_initializer,
  MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  switch (field_id) {
    case 0:
    { // int idx;
      return SageBuilder::buildIntVal(input->id);
    }
    case 1:
    { // enum tile_kind_e { e_tile_static, e_tile_dynamic } kind;
      switch (input->kind) {
        case MDCG::KLT::Descriptor::e_static_tile:  return SageBuilder::buildIntVal(0);
        case MDCG::KLT::Descriptor::e_dynamic_tile: return SageBuilder::buildIntVal(1);
        default: assert(false);
      }
    }
    case 2:
    { // int param;
      return SageBuilder::buildIntVal(input->nbr_it);
    }
    default:
      assert(false);
  }
}

template <>
SgExpression * LoopDesc<Hlang>::createFieldInitializer(
  const MDCG::StaticInitializer & static_initializer,
  MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  switch (field_id) {
    case 0:
      /// size_t id;
      return SageBuilder::buildIntVal(input->id);
    case 1:
      /// size_t num_tiles;
      return SageBuilder::buildIntVal(input->tiles.size());
    case 2:
    { /// struct tile_desc_t * tile_desc;
      std::ostringstream decl_name; decl_name << "tile_" << &input;
      MDCG::Model::class_t field_class = StaticInitializer::getBaseClassForPointerOnClass(element, "tile_desc", "tile_desc_t");
      return static_initializer.createArrayPointer<TileDesc<Hlang> >(field_class, input->tiles.size(), input->tiles.begin(), input->tiles.end(), file_id, decl_name.str());
    }
    default:
      assert(false);
  }
}

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
    { // int num_data;
      return SageBuilder::buildIntVal(input->getArguments().datas.size());
    }
    case 1:
    { // int num_param;
      return SageBuilder::buildIntVal(input->getArguments().parameters.size());
    }
    case 2:
    { // int num_scalar;
      return SageBuilder::buildIntVal(input->getArguments().scalars.size());
    }
    case 3:
    { // int num_loops;
      return SageBuilder::buildIntVal(kernel->loops.size());
    }
    case 4:
    { // int num_tiles;
      return SageBuilder::buildIntVal(kernel->tiles.size());
    }
    case 5:
    { // struct loop_desc_t * loop_desc;
      std::ostringstream decl_name; decl_name << "loop_" << kernel;
      MDCG::Model::class_t field_class = StaticInitializer::getBaseClassForPointerOnClass(element, "loop_desc", "loop_desc_t");
      return static_initializer.createArrayPointer<LoopDesc<Hlang> >(field_class, kernel->loops.size(), kernel->loops.begin(), kernel->loops.end(), file_id, decl_name.str());
    }
    case 6:
    { // kernel_func_ptr kernel_ptr;
      MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(kernel->kernel_name, Runtime<Hlang, Klang>::host_api.getKernelFnctPtrType(), NULL, NULL, file_id, false, true);
      MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = static_initializer.getDriver().build<SgVariableDeclaration>(var_decl_desc);

      SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(var_decl_res.symbol->get_declaration()->get_parent());
        decl_stmt->get_declarationModifier().unsetDefault();
        decl_stmt->get_declarationModifier().get_storageModifier().setExtern();

      return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(var_decl_res.symbol));
    }
    default:
      assert(false);
  }
}

} // namespace MDCG::KLT

} // namespace MDCG

#endif /* __MDCG_KLT_MODEL_HPP__ */
