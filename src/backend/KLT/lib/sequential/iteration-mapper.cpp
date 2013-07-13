
#include "KLT/Sequential/iteration-mapper.hpp"

namespace KLT {

namespace Core {

SgBasicBlock * IterationMap<Kernel>::generateBodyForMappedIterationDomain(
  const Kernel::argument_symbol_maps_t & argument_symbol_maps,
  SgBasicBlock * kernel_body,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
) const {
  assert(false); // TODO
}

std::pair<SgExpression *, SgExpression *> IterationMap<Kernel>::generateIteratorBounds(
  const Kernel::argument_symbol_maps_t & argument_symbol_maps,
  const Kernel::coordinate_symbols_t & coordinate_symbols,
  SgVariableSymbol * iterator
) const {
  assert(false); // TODO
}

void IterationMapper<Kernel>::generateShapes(
  Kernel::loop_distribution_t * loop_distribution,
  std::set<IterationMap<Kernel> *> & shapes
) const {
  assert(false); // TODO
}

}

}
