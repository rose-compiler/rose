
#ifndef __MFB_KLT_DRIVER_HPP__
#define __MFB_KLT_DRIVER_HPP__

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/function-declaration.hpp"

#include "KLT/utils.hpp"

#include <vector>
#include <map>

class SgStatement;

namespace MFB {

namespace KLT {

typedef ::KLT::Kernel::kernel_t kernel_t;

typedef ::KLT::Descriptor::kernel_t kernel_desc_t;
typedef ::KLT::Descriptor::data_t data_desc_t;
typedef ::KLT::Descriptor::loop_t loop_desc_t;
typedef ::KLT::Descriptor::tile_t tile_desc_t;

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
    typedef ::KLT::Descriptor::data_t data_t;
    typedef ::MFB::KLT::node_t node_t;

    typedef std::map<vsym_t *, vsym_t *> vsym_translation_t;
    typedef std::map<data_t *, vsym_t *> data_translation_t;

    typedef std::vector<data_t *> data_list_t;
    typedef std::vector<vsym_t *> vsym_list_t;

  public:
    Driver(SgProject * project_);
    virtual ~Driver();

    struct kernel_desc_t {
      node_t * root;

      const vsym_list_t & parameters;
      const data_list_t & data;

      ::KLT::Runtime * runtime;

      kernel_desc_t(node_t * root, const vsym_list_t & parameters, const data_list_t & data, ::KLT::Runtime * runtime);
    };

    typedef SgStatement * sg_stmt_ptr;

    struct looptree_desc_t {
      const ::KLT::Utils::symbol_map_t & symbol_map;
      ::KLT::Runtime * runtime;
      node_t * node;

      looptree_desc_t(node_t * node_, ::KLT::Runtime * runtime_, const ::KLT::Utils::symbol_map_t & symbol_map_);
    };

    template <class Object>
    typename ::MFB::KLT::KLT<Object>::build_result_t build(typename ::MFB::KLT::KLT<Object>::object_desc_t const & object) { return ::MFB::KLT::KLT<Object>::build(*this, object); }
};

namespace KLT {

template <>
struct KLT<kernel_t> {
  typedef ::KLT::Descriptor::kernel_t * build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::kernel_desc_t object_desc_t;
  static build_result_t build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const object_desc_t & object);
private:
  static size_t kernel_cnt;
  static ::KLT::Descriptor::kernel_t * buildKernelDesc(const std::string & kernel_prefix);
  static ::MFB::Sage<SgFunctionDeclaration>::build_result_t buildKernelDecl(::KLT::Descriptor::kernel_t & res, ::KLT::Runtime * runtime);
};

template <>
struct KLT<node_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t object_desc_t;
  static build_result_t build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const object_desc_t & object);
};

template <>
struct KLT<block_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t object_desc_t;
  static build_result_t build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const object_desc_t & object);
};

template <>
struct KLT<cond_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t object_desc_t;
  static build_result_t build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const object_desc_t & object);
};

template <>
struct KLT<loop_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t object_desc_t;
  static build_result_t build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const object_desc_t & object);
};

template <>
struct KLT<tile_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t object_desc_t;
  static build_result_t build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const object_desc_t & object);
};

template <>
struct KLT<stmt_t> {
  typedef Driver< ::MFB::KLT::KLT>::sg_stmt_ptr build_result_t;
  typedef Driver< ::MFB::KLT::KLT>::looptree_desc_t object_desc_t;
  static build_result_t build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const object_desc_t & object);
};

} // namespace MFB::KLT

} // namespace MFB

#endif /*  __MFB_KLT_DRIVER_HPP__ */

