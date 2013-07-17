
#include "KLT/Core/iteration-mapper.hpp"

#include "KLT/Sequential/kernel.hpp"

#include <cassert>

#include "sage3basic.h"

namespace KLT {

namespace Core {

template <>
std::pair<SgExpression *, SgExpression *> IterationMap<Sequential::Kernel>::generateIteratorBounds(
  const Sequential::Kernel::local_symbol_maps_t & local_symbol_maps,
  const Sequential::Kernel::coordinate_symbols_t & coordinate_symbols,
  LoopTrees::loop_t * loop, unsigned int loop_depth
) const {
  std::pair<SgExpression *, SgExpression *> result(
    SageInterface::copyExpression(loop->lower_bound),
    SageInterface::copyExpression(loop->upper_bound)
  );

  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(result.first);
  std::vector<SgVarRefExp *> var_refs_ = SageInterface::querySubTree<SgVarRefExp>(result.second);
  var_refs.insert(var_refs.end(), var_refs_.begin(), var_refs_.end());
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_parameter = local_symbol_maps.parameters.find(var_ref->get_symbol());
    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_iterator  = local_symbol_maps.iterators.find(var_ref->get_symbol());
    if (it_parameter != local_symbol_maps.parameters.end())
      SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(it_parameter->second));
    else if (it_iterator != local_symbol_maps.iterators.end())
      SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(it_iterator->second));
    else assert(false);
  }

  return result;
}

template <>
void IterationMap<Sequential::Kernel>::generateDimensions(
  const Sequential::Kernel::local_symbol_maps_t & local_symbol_maps,
  Sequential::Kernel::dimensions_t & dimensions
) const {}

template <>
void IterationMap<Sequential::Kernel>::generateCoordinates(
  Sequential::Kernel::coordinate_symbols_t & coordinate_symbols,
  SgBasicBlock * kernel_body,
  const Sequential::Kernel::dimensions_t & dimensions
) const {}


}

}
