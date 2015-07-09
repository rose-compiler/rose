
#ifndef __MDCG_TILEK_HPP_
#define __MDCG_TILEK_HPP_

#include "MDCG/static-initializer.hpp"

#include "KLT/Language/none.hpp"
#include "KLT/Runtime/tilek.hpp"

typedef ::KLT::Language::None Language;
typedef ::KLT::Runtime::TileK Runtime;
typedef ::DLX::KLT::Annotation< ::DLX::TileK::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

namespace MDCG {

namespace TileK {

struct TileDesc {
  typedef Runtime::tile_desc_t * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  );
};

struct LoopDesc {
  typedef Runtime::loop_desc_t * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  );
};

struct KernelDesc {
  typedef Kernel * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  );
};

}

}

#endif /* __MDCG_TILEK_HPP_ */

