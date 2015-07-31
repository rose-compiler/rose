
#ifndef __KLT_KERNEL_HPP__
#define __KLT_KERNEL_HPP__

#include <vector>
#include <map>

class SgVariableSymbol;
class SgForStatement;
class SgStatement;
namespace KLT {
  namespace Descriptor {
    struct data_t;
  }
  namespace LoopTree {
    struct node_t;
    struct loop_t;
  }

namespace Kernel {

class kernel_t {
  public:
    typedef ::KLT::Descriptor::data_t data_t;
    typedef ::KLT::LoopTree::node_t node_t;
    typedef ::KLT::LoopTree::loop_t loop_t;
    typedef SgVariableSymbol vsym_t;

    typedef std::vector<vsym_t *> vsym_list_t;
    typedef std::vector<data_t *> data_list_t;

  public:
    node_t * root;

    vsym_list_t parameters;
    data_list_t data;

  protected:
    kernel_t();

  public:
    static kernel_t * extract(SgStatement * stmt, const data_list_t & data, std::map<SgForStatement *, size_t> & loop_map);
};

}

}

#endif /* __KLT_KERNEL_HPP__ */

