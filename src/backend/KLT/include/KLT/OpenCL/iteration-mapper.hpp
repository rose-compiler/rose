
#ifndef __KLT_OPENCL_ITERATION_MAPPER_HPP__
#define __KLT_OPENCL_ITERATION_MAPPER_HPP__

#include "KLT/Core/iteration-mapper.hpp"

#include "KLT/OpenCL/kernel.hpp"

namespace KLT {

namespace Core {

template <>
class IterationMap<Kernel> {
  protected:
    unsigned int p_number_dims;

    std::vector<SgExpression *> p_global_dims;
    std::vector<SgExpression *> p_local_dims;
    
    bool p_have_local_dims;

    Kernel::loop_distribution_t * p_loop_distribution;

  protected:
    virtual std::pair<SgExpression *, SgExpression *> generateIteratorBounds(
      const Kernel::argument_symbol_maps_t & argument_symbol_maps,
      const Kernel::coordinate_symbols_t & coordinate_symbols,
      LoopTrees::loop_t * loop, unsigned int loop_depth
    ) const;

  public:
    IterationMap(unsigned int number_dims, bool have_local_dims, Kernel::loop_distribution_t * loop_distribution);
    virtual ~IterationMap();

    unsigned int getNumberDimensions() const;

    const std::vector<SgExpression *> & getGlobalDimensions() const;

    const std::vector<SgExpression *> & getLocalDimensions() const;

    bool haveLocalDimensions() const;

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

#endif /* __KLT_OPENCL_ITERATION_MAPPER_HPP__ */
