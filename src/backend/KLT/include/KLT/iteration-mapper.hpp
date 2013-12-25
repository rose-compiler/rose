
#ifndef __KLT_ITERATION_MAPPER_HPP__
#define __KLT_ITERATION_MAPPER_HPP__

#include "KLT/loop-trees.hpp"

#include <cassert>

#include "sage3basic.h"

class SgVariableSymbol;
class SgBasicBlock;

namespace KLT {

template <class Annotation> class Data;
template <class Annotation> class LoopTrees;
template <class Annotation, class Language, class Runtime> class Kernel;

/*!
 * \addtogroup grp_klt_itmap
 * @{
*/

template <class Annotation, class Language, class Runtime>
class IterationMap {
  protected:
    typename Kernel<Annotation, Language, Runtime>::loop_mapping_t * p_loop_mapping;

    std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t> p_loop_shapes;

  public:
    IterationMap(typename Kernel<Annotation, Language, Runtime>::loop_mapping_t * loop_mapping);

    /** Generate a basic block where the iterators take  all the value they are supposed to for a given set of coordinate
     *  \param loop_nest
     *  \param symbol_maps
     *  \param kernel_body
     *  \param iter_to_local 
     * \return a basic block which will be executed for all valid value of the iterator
     */
    SgBasicBlock * generateBodyForMappedIterationDomain(
      typename Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
      SgBasicBlock * kernel_body
    ) const;
};

template <class Annotation, class Language, class Runtime>
IterationMap<Annotation, Language, Runtime>::IterationMap(
  typename Kernel<Annotation, Language, Runtime>::loop_mapping_t * loop_mapping
) :
  p_loop_mapping(loop_mapping)
{}

template <class Annotation, class Language, class Runtime>
class IterationMapper {
  public:
    /** Create iteration mapping for a given loop distribution
     *  \param loop_distribution
     *  \param shapes
     */
    void generateShapes(
      typename Kernel<Annotation, Language, Runtime>::loop_mapping_t * loop_mapping,
      std::set<IterationMap<Annotation, Language, Runtime> *> & shapes
    ) const;
};

/** @} */

}

#endif /*  __KLT_ITERATION_MAPPER_HPP__ */

