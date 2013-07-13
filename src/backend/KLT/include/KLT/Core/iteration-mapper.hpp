
#ifndef __KLT_CORE_ITERATION_MAPPER_HPP__
#define __KLT_CORE_ITERATION_MAPPER_HPP__

#include "KLT/Core/loop-trees.hpp"

namespace KLT {

namespace Core {

template <class Kernel>
class IterationMap {
  protected:
    /**
     * \param argument_symbol_maps 
     * \param coordinate_symbols
     * \param loop pointer to a loop object
     * \param loop_depth position of the loop in the loop nest
     * \return res.first: lower bound of the interval describe by the thread, res.second: size of the interval (if NULL it is assumed to be 1)
     */
    virtual std::pair<SgExpression *, SgExpression *> generateIteratorBounds(
      typename const Kernel::argument_symbol_maps_t & argument_symbol_maps,
      typename const Kernel::coordinate_symbols_t & coordinate_symbols,
      LoopTrees::loop_t * loop, unsigned int loop_depth
    ) const;

  public:
    /** Generate a basic block where the iterators take  all the value they are supposed to for a given set of coordinate
     *  \param loop_nest
     *  \param symbol_maps
     *  \param kernel_body
     *  \param iter_to_local 
     * \return a basic block which will be executed for all valid value of the iterator
     */
    virtual SgBasicBlock * generateBodyForMappedIterationDomain(
      typename const Kernel::argument_symbol_maps_t & argument_symbol_maps,
      SgBasicBlock * kernel_body,
      std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
    ) const;
};

template <class Kernel>
class IterationMapper {
  public:
    /** Create iteration mapping for a given loop distribution
     *  \param loop_distribution
     *  \param shapes
     */
    virtual void generateShapes(
      typename Kernel::loop_distribution_t * loop_distribution,
      std::set<IterationMap<Kernel> *> & shapes
    ) const;
};

}

}

#ifndef __NO_TEMPLATE_DEFINITION__
#include "KLT/Core/iteration-mapper.tpp"
#endif

#endif /*  __KLT_CORE_ITERATION_MAPPER_HPP__ */

