
#include "sage3basic.h"

#include "MFB/KLT/driver.hpp"
#include "KLT/descriptor.hpp"

namespace MFB {

Driver< ::MFB::KLT::KLT>::Driver(SgProject * project_) : ::MFB::Driver<Sage>(project_) {}
Driver< ::MFB::KLT::KLT>::~Driver() {}

Driver< ::MFB::KLT::KLT>::kernel_desc_t::kernel_desc_t(node_t * roots_, const vsym_list_t & parameters_, const data_list_t & data_, ::KLT::Runtime * runtime_, ::MFB::file_id_t file_id_) :
  roots(roots_), parameters(parameters_), data(data_), runtime(runtime_), file_id(file_id_) {}

namespace KLT {

/////////////////////////////////////////////////////////////////////////////
///// kernel_t

KLT<kernel_t>::build_result_t KLT<kernel_t>::build(const KLT<kernel_t>::object_desc_t & object) {
  size_t id = 0; // TODO
  std::string kernel_name = ""; // TODO

  ::KLT::Descriptor::kernel_t res(id, kernel_name);

  // TODO std::vector<loop_t *> res.loops;
  // TODO std::vector<tile_t *> res.tiles;

  return res;
}

/////////////////////////////////////////////////////////////////////////////
///// LoopTree::block_t

KLT<block_t>::build_result_t KLT<block_t>::build(const KLT<block_t>::object_desc_t & object) {
// const vsym_translation_t & object.iterators;
// const vsym_translation_t & object.parameters;
// const data_translation_t & object.datas;
// const vsym_t * object.loop_context;
// const vsym_t * object.data_context;
// node_t * object.node;

  SgForStatement * for_stmt = NULL;

  // TODO

  return for_stmt;
}

///// LoopTree::cond_t

KLT<cond_t>::build_result_t KLT<cond_t>::build(const KLT<cond_t>::object_desc_t & object) {
// const vsym_translation_t & object.iterators;
// const vsym_translation_t & object.parameters;
// const data_translation_t & object.datas;
// const vsym_t * object.loop_context;
// const vsym_t * object.data_context;
// node_t * object.node;

  SgForStatement * for_stmt = NULL;

  // TODO

  return for_stmt;
}

///// LoopTree::loop_t

KLT<loop_t>::build_result_t KLT<loop_t>::build(const KLT<loop_t>::object_desc_t & object) {
// const vsym_translation_t & object.iterators;
// const vsym_translation_t & object.parameters;
// const data_translation_t & object.datas;
// const vsym_t * object.loop_context;
// const vsym_t * object.data_context;
// node_t * object.node;

  SgForStatement * for_stmt = NULL;

  // TODO

  return for_stmt;
}

///// LoopTree::tile_t

KLT<tile_t>::build_result_t KLT<tile_t>::build(const KLT<tile_t>::object_desc_t & object) {
// const vsym_translation_t & object.iterators;
// const vsym_translation_t & object.parameters;
// const data_translation_t & object.datas;
// const vsym_t * object.loop_context;
// const vsym_t * object.data_context;
// node_t * object.node;

  SgForStatement * for_stmt = NULL;

  // TODO

  return for_stmt;
}

///// LoopTree::stmt_t

KLT<stmt_t>::build_result_t KLT<stmt_t>::build(const KLT<stmt_t>::object_desc_t & object) {
// const vsym_translation_t & object.iterators;
// const vsym_translation_t & object.parameters;
// const data_translation_t & object.datas;
// const vsym_t * object.loop_context;
// const vsym_t * object.data_context;
// node_t * object.node;

  SgStatement * stmt = NULL;

  // TODO

  return stmt;
}

} // namespace MFB::KLT

} // namespace MFB

