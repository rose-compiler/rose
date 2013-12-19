
#ifdef __NO_TEMPLATE_INSTANTIATION__
#error "Entering a template definition file when the macro __NO_TEMPLATE_INSTANTIATION__ is defined"
#endif

#include "sage3basic.h"

#include <cassert>

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

template <class Kernel>
IterationMap<Kernel>::IterationMap(typename Kernel::loop_mapping_t * loop_mapping) :
  p_loop_mapping(loop_mapping)
{}

template <class Kernel>
SgBasicBlock * IterationMap<Kernel>::generateBodyForMappedIterationDomain(
  typename Kernel::local_symbol_maps_t & local_symbol_maps,
  SgBasicBlock * kernel_body,
  const typename Kernel::dimensions_t & dimensions
) const {
  SgBasicBlock * result = kernel_body;
  std::list<LoopTrees::loop_t *>::const_iterator it_loop;

  typename Kernel::coordinate_symbols_t coordinate_symbols;
  generateCoordinates(coordinate_symbols, kernel_body, dimensions);

  unsigned long loop_depth = 0;
  for (it_loop = p_loop_mapping->loop_nest.begin(); it_loop != p_loop_mapping->loop_nest.end(); it_loop++) {
    LoopTrees::loop_t * loop = *it_loop;
    std::pair<SgExpression *, SgExpression *> bounds = IterationMap<Kernel>::generateIteratorBounds(local_symbol_maps, coordinate_symbols, loop, loop_depth++);
    assert(bounds.first != NULL);

    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_iter = local_symbol_maps.iterators.find(loop->iterator);
    assert(it_iter != local_symbol_maps.iterators.end());
    SgVariableSymbol * local_iter_sym = it_iter->second;

    if (bounds.second != NULL) {
      SgBasicBlock * body = SageBuilder::buildBasicBlock();
      SgForStatement * for_stmt = SageBuilder::buildForStatement(
        SageBuilder::buildAssignStatement(SageBuilder::buildVarRefExp(local_iter_sym), bounds.first),
        SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(SageBuilder::buildVarRefExp(local_iter_sym), bounds.second)),
        SageBuilder::buildPlusPlusOp(SageBuilder::buildVarRefExp(local_iter_sym)),
        body
      );
      SageInterface::appendStatement(for_stmt, result);
      result = body;
      assert(body->get_parent() != NULL);
    }
    else {
      SgExprStatement * expr_stmt = SageBuilder::buildAssignStatement(SageBuilder::buildVarRefExp(local_iter_sym), bounds.first);
      SageInterface::appendStatement(expr_stmt, result);
    }
  }

  return result;
}

template <class Kernel>
void IterationMapper<Kernel>::generateShapes(
  typename Kernel::loop_mapping_t * loop_mapping,
  std::set<IterationMap<Kernel> * > & shapes
) const {
  shapes.insert(new IterationMap<Kernel>(loop_mapping));
}

/** @} */

}

}

