
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/looptree.hpp"
#include "KLT/Core/descriptor.hpp"

#include <set>

#include <cassert>

namespace KLT {

namespace Kernel {

kernel_t::kernel_t() : root(NULL), parameters(), data() {}

kernel_t::kernel_t(node_t * root_, const vsym_list_t & parameters_, const data_list_t & data_) :
  root(root_), parameters(parameters_), data(data_) {}

kernel_t * kernel_t::extract(SgStatement * stmt, const data_list_t & data_, std::map<SgForStatement *, size_t> & loop_map) {
  typedef SgVariableSymbol vsym_t;
  typedef std::set<vsym_t *> vsym_set_t;

  kernel_t * res = new kernel_t();
    res->data = data_;

  data_list_t::const_iterator it_data;
  vsym_set_t data_syms;
  for (it_data = res->data.begin(); it_data != res->data.end(); it_data++)
    data_syms.insert((*it_data)->symbol);

  LoopTree::extraction_context_t ctx(loop_map, data_syms);

  assert(stmt != NULL);
  res->root = LoopTree::node_t::extract(stmt, ctx);
  assert(res->root != NULL);

  res->parameters.insert(res->parameters.end(), ctx.getParameters().begin(), ctx.getParameters().end());

  return res;
}

}

}

