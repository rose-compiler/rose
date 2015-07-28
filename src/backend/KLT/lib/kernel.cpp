
#include "KLT/kernel.hpp"
#include "KLT/looptree.hpp"
#include "KLT/data.hpp"

#include <set>

#include <cassert>

namespace KLT {

namespace Kernel {

kernel_t::kernel_t(SgStatement * stmt, const data_list_t & data_) :
   root(NULL), parameters(), data(data_)
{
  typedef SgVariableSymbol vsym_t;
  typedef std::set<vsym_t *> vsym_set_t;

  data_list_t::const_iterator it_data;
  vsym_set_t data_syms;
  for (it_data = data.begin(); it_data != data.end(); it_data++)
    data_syms.insert((*it_data)->symbol);

  LoopTree::extraction_context_t ctx(loop_map, data_syms);

  assert(stmt != NULL);
  root = LoopTree::node_t::extract(stmt, ctx);
  assert(root != NULL);

  parameters.insert(parameters.end(), ctx.getParameters().begin(), ctx.getParameters().end());
}

}

}

