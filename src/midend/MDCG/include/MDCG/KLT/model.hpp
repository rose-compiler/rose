
#ifndef __MDCG_KLT_MODEL_HPP__
#define __MDCG_KLT_MODEL_HPP__

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
      return SageBuilder::buildIntVal(input->kind);
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
      MDCG::Model::class_t field_class = StaticInitializer::getBaseClassForPointerOnClass(element, "tile_desc", "klt_tile_desc_t");
      return static_initializer.createArrayPointer<TileDesc<Hlang> >(field_class, input->tiles.size(), input->tiles.begin(), input->tiles.end(), file_id, decl_name.str());
    }
    default:
      assert(false);
  }
}

} // namespace MDCG::KLT

} // namespace MDCG

#endif /* __MDCG_KLT_MODEL_HPP__ */
