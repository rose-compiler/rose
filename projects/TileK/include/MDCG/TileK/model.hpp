
#ifndef __MDCG_TILEK_HPP_
#define __MDCG_TILEK_HPP_

#include "MDCG/static-initializer.hpp"
#include "KLT/TileK/tilek.hpp"

typedef ::KLT::Language::None Language;
typedef ::KLT::Runtime::TileK Runtime;
typedef ::DLX::KLT_Annotation< ::DLX::TileK::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

namespace MDCG {

namespace TileK {

//struct tile_desc_t tiles_loop_1[2] = {
//  { 0, e_tile_static,   2 },
//  { 1, e_tile_dynamic,  0 }
//};

//struct tile_desc_t tiles_loop_2[2] = {
//  { 2, e_tile_static,   2 },
//  { 3, e_tile_dynamic,  0 }
//};

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

//struct loop_desc_t loop_desc[2] = {
//  { 0, 2, tiles_loop_1 },
//  { 1, 2, tiles_loop_2 }
//};

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

//struct kernel_desc_t kernel_desc[1] = {
//  { 2, 2, 2, 4, loop_desc, &kernel_0 }
//};

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

