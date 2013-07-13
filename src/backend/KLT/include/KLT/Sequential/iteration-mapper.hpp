
#ifndef __KLT_SEQUENTIAL_ITERATION_MAPPER_HPP__
#define __KLT_SEQUENTIAL_ITERATION_MAPPER_HPP__

#include "KLT/Core/iteration-mapper.hpp"

#include "KLT/Sequential/kernel.hpp"

namespace KLT {

namespace Core {

template <>
class IterationMap<Kernel> {
  protected:
    virtual std::pair<SgExpression *, SgExpression *> generateIteratorBounds(
      const Kernel::argument_symbol_maps_t & argument_symbol_maps,
      const Kernel::coordinate_symbols_t & coordinate_symbols,
      LoopTrees::loop_t * loop, unsigned int loop_depth
    ) const;

  public:
    virtual SgBasicBlock * generateBodyForMappedIterationDomain(
      const Kernel::argument_symbol_maps_t & argument_symbol_maps,
      SgBasicBlock * kernel_body,
      std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
    ) const;
};

template <>
class IterationMapper<Kernel> {
  public:
    virtual void generateShapes(
      Kernel::loop_distribution_t * loop_distribution,
      std::set<IterationMap<Kernel> *> & shapes
    ) const;
};

}

}

#endif /* __KLT_SEQUENTIAL_ITERATION_MAPPER_HPP__ */
