
#ifndef __KLT_ITERATION_MAPPER_HPP__
#define __KLT_ITERATION_MAPPER_HPP__

#include "KLT/loop-trees.hpp"

#include <cassert>

#include "sage3basic.h"

class SgVariableSymbol;
class SgBasicBlock;

namespace KLT {

template <class Annotation> class Data;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class Kernel;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation> class LoopTrees;

/*!
 * \addtogroup grp_klt_itmap
 * @{
*/

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
class IterationMap {
  protected:
    typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * p_loop_mapping;

  protected:
    /**
     * \param argument_symbol_maps 
     * \param coordinate_symbols
     * \param loop pointer to a loop object
     * \param loop_depth position of the loop in the loop nest
     * \return res.first: lower bound of the interval describe by the thread, res.second: size of the interval (if NULL it is assumed to be 1)
     */
    virtual std::pair<SgExpression *, SgExpression *> generateIteratorBounds(
      const typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::local_symbol_maps_t  & local_symbol_maps,
      const typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::coordinate_symbols_t & coordinate_symbols,
      typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t * loop,
      unsigned int loop_depth
    ) const;

    void generateCoordinates(
      typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::coordinate_symbols_t & coordinate_symbols,
      SgBasicBlock * kernel_body,
      const typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::dimensions_t & dimensions
    ) const;

  public:
    IterationMap(typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * loop_mapping);

    /** Generate a basic block where the iterators take  all the value they are supposed to for a given set of coordinate
     *  \param loop_nest
     *  \param symbol_maps
     *  \param kernel_body
     *  \param iter_to_local 
     * \return a basic block which will be executed for all valid value of the iterator
     */
    virtual SgBasicBlock * generateBodyForMappedIterationDomain(
      typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
      SgBasicBlock * kernel_body,
      const typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::dimensions_t & dimensions
    ) const;

    /**
     * \param 
     * \return 
     */
    virtual void generateDimensions(
      const typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
      typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::dimensions_t & dimensions
    ) const;
};

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
IterationMap<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::IterationMap(
  typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * loop_mapping
) :
  p_loop_mapping(loop_mapping)
{}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
SgBasicBlock * IterationMap<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::generateBodyForMappedIterationDomain(
  typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
  SgBasicBlock * kernel_body,
  const typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::dimensions_t & dimensions
) const {
  SgBasicBlock * result = kernel_body;
  typename std::list<typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t *>::const_iterator it_loop;

  typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::coordinate_symbols_t coordinate_symbols;
  generateCoordinates(coordinate_symbols, kernel_body, dimensions);

  unsigned long loop_depth = 0;
  for (it_loop = p_loop_mapping->loop_nest.begin(); it_loop != p_loop_mapping->loop_nest.end(); it_loop++) {
    typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t * loop = *it_loop;
    std::pair<SgExpression *, SgExpression *> bounds =
        IterationMap<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::generateIteratorBounds(
            local_symbol_maps, coordinate_symbols, loop, loop_depth++
        );
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

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
class IterationMapper {
  public:
    /** Create iteration mapping for a given loop distribution
     *  \param loop_distribution
     *  \param shapes
     */
    virtual void generateShapes(
      typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * loop_mapping,
      std::set<IterationMap<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> & shapes
    ) const;
};

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
void IterationMapper<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::generateShapes(
  typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * loop_mapping,
  std::set<IterationMap<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> & shapes
) const {
  shapes.insert(new IterationMap<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>(loop_mapping));
}

/** @} */

}

#endif /*  __KLT_ITERATION_MAPPER_HPP__ */

