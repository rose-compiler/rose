
#ifndef __MFB_KLT_DRIVER_HPP__
#define __MFB_KLT_DRIVER_HPP__

#include "MFB/Sage/driver.hpp"

#include <vector>
#include <map>

class SgStatement;
namespace KLT {
  class Runtime;
  namespace Descriptor {
    struct kernel_t;
  }
  namespace Kernel {
    struct kernel_t;
  }
  namespace Data {
    struct data_t;
  }
  namespace LoopTree {
    struct node_t;
    struct block_t;
    struct cond_t;
    struct loop_t;
    struct tile_t;
    struct stmt_t;
  }
}

namespace MFB {

namespace KLT {

typedef ::KLT::Kernel::kernel_t kernel_t;

typedef ::KLT::Data::data_t data_t;

typedef ::KLT::LoopTree::node_t node_t;
typedef ::KLT::LoopTree::block_t block_t;
typedef ::KLT::LoopTree::cond_t cond_t;
typedef ::KLT::LoopTree::loop_t loop_t;
typedef ::KLT::LoopTree::tile_t tile_t;
typedef ::KLT::LoopTree::stmt_t stmt_t;

typedef SgVariableSymbol vsym_t;

template <class Object>
struct KLT {};

} // namespace MFB::KLT

template <>
class Driver< ::MFB::KLT::KLT> : public Driver< ::MFB::Sage> {
  public:
    typedef ::MFB::KLT::vsym_t vsym_t;
    typedef ::MFB::KLT::data_t data_t;
    typedef ::MFB::KLT::node_t node_t;

    typedef std::map<vsym_t *, vsym_t *> vsym_translation_t;
    typedef std::map<data_t *, vsym_t *> data_translation_t;

    typedef std::vector<data_t *> data_list_t;
    typedef std::vector<vsym_t *> vsym_list_t;

  public:
    Driver(SgProject * project_);
    virtual ~Driver();

    struct kernel_desc_t {
      node_t * roots;

      const vsym_list_t & parameters;
      const data_list_t & data;

      ::KLT::Runtime * runtime;
      ::MFB::file_id_t file_id;

      kernel_desc_t(node_t * roots, const vsym_list_t & parameters, const data_list_t & data, ::KLT::Runtime * runtime, ::MFB::file_id_t file_id);
    };

    typedef SgStatement * sg_stmt_ptr;

    template <class node_t>
    struct looptree_desc_t {
      const vsym_translation_t & iterators;
      const vsym_translation_t & parameters;
      const data_translation_t & datas;

      const vsym_t * loop_context;
      const vsym_t * data_context;

      node_t * node;

      looptree_desc_t(
        const vsym_translation_t & iterators_, const vsym_translation_t & parameters_, const data_translation_t & datas_,
        const vsym_t * loop_context_, const vsym_t * data_context_
      ) :
        iterators(iterators_), parameters(parameters_), datas(datas_),
        loop_context(loop_context_), data_context(data_context_), node(NULL)
      {}

      looptree_desc_t(node_t * node_, const looptree_desc_t & desc) :
        iterators(desc.iterators), parameters(desc.parameters), datas(desc.datas),
        loop_context(desc.loop_context), data_context(desc.data_context), node(node_)
      {}
    };

    template <class Object>
    typename ::MFB::KLT::KLT<Object>::build_result_t build(typename ::MFB::KLT::KLT<Object>::object_desc_t const & object) { return ::MFB::KLT::KLT<Object>::build(object); }
};

namespace KLT {

template <>
struct KLT<kernel_t> {
  typedef ::KLT::Descriptor::kernel_t build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::kernel_desc_t object_desc_t;
  static build_result_t build(const object_desc_t & object);
};

template <>
struct KLT<block_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t<block_t> object_desc_t;
  static build_result_t build(const object_desc_t & object);
};

template <>
struct KLT<cond_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t<cond_t> object_desc_t;
  static build_result_t build(const object_desc_t & object);
};

template <>
struct KLT<loop_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t<loop_t> object_desc_t;
  static build_result_t build(const object_desc_t & object);
};

template <>
struct KLT<tile_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t<tile_t> object_desc_t;
  static build_result_t build(const object_desc_t & object);
};

template <>
struct KLT<stmt_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t<stmt_t> object_desc_t;
  static build_result_t build(const object_desc_t & object);
};

} // namespace MFB::KLT

} // namespace MFB

#endif /*  __MFB_KLT_DRIVER_HPP__ */

