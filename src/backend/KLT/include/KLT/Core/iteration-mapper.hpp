
#ifndef __KLT_CORE_ITERATION_MAPPER_HPP__
#define __KLT_CORE_ITERATION_MAPPER_HPP__

#include "KLT/Core/loop-trees.hpp"

class SgVariableSymbol;
class SgBasicBlock;

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

template <class Kernel>
class IterationMap {
  protected:
    typename Kernel::loop_mapping_t * p_loop_mapping;

  protected:
    /**
     * \param argument_symbol_maps 
     * \param coordinate_symbols
     * \param loop pointer to a loop object
     * \param loop_depth position of the loop in the loop nest
     * \return res.first: lower bound of the interval describe by the thread, res.second: size of the interval (if NULL it is assumed to be 1)
     */
    virtual std::pair<SgExpression *, SgExpression *> generateIteratorBounds(
      const typename Kernel::local_symbol_maps_t & local_symbol_maps,
      const typename Kernel::coordinate_symbols_t & coordinate_symbols,
      LoopTrees::loop_t * loop, unsigned int loop_depth
    ) const;

    void generateCoordinates(
      typename Kernel::coordinate_symbols_t & coordinate_symbols,
      SgBasicBlock * kernel_body,
      const typename Kernel::dimensions_t & dimensions
    ) const;

  public:
    IterationMap(typename Kernel::loop_mapping_t * loop_mapping);

    /** Generate a basic block where the iterators take  all the value they are supposed to for a given set of coordinate
     *  \param loop_nest
     *  \param symbol_maps
     *  \param kernel_body
     *  \param iter_to_local 
     * \return a basic block which will be executed for all valid value of the iterator
     */
    virtual SgBasicBlock * generateBodyForMappedIterationDomain(
      typename Kernel::local_symbol_maps_t & local_symbol_maps,
      SgBasicBlock * kernel_body,
      const typename Kernel::dimensions_t & dimensions
    ) const;

    /**
     * \param 
     * \return 
     */
    virtual void generateDimensions(
      const typename Kernel::local_symbol_maps_t & local_symbol_maps,
      typename Kernel::dimensions_t & dimensions
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
      typename Kernel::loop_mapping_t * loop_mapping,
      std::set<IterationMap<Kernel> *> & shapes
    ) const;
};

/** @} */

}

}

#ifndef __NO_TEMPLATE_DEFINITION__
#include "KLT/Core/iteration-mapper.tpp"
#endif

#endif /*  __KLT_CORE_ITERATION_MAPPER_HPP__ */

