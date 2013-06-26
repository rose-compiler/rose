
#include "KLT/Core/data-flow.hpp"

namespace KLT {

namespace Core {

void DataFlow::apply(
  const std::set<Data *> & datas_in,
  const std::set<Data *> & datas_out,
  const std::set<Data *> & datas_local,
  const std::list<Kernel *> & kernels,
  const std::map<Kernel *, LoopTrees::node_t *> & kernels_map
) const {
  // TODO
}

}

}

