
#ifndef __KLT_CORE_DATA_FLOW_HPP__
#define __KLT_CORE_DATA_FLOW_HPP__

#include <list>

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

class Data;
class LoopTrees;

template <class Kernel>
class DataFlow {
  public:
    virtual void generateFlowSets(
      const LoopTrees & loop_trees,
      const std::list<Kernel *> & kernels
    ) const;
};

/** @} */

}

}

#ifndef __NO_TEMPLATE_INSTANTIATION__
#include "KLT/Core/data-flow.tpp"
#endif

#endif /* __KLT_CORE_DATA_FLOW_HPP__ */

