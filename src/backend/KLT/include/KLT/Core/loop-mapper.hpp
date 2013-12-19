
#ifndef __KLT_CORE_LOOP_MAPPER_HPP__
#define __KLT_CORE_LOOP_MAPPER_HPP__

#include <list>
#include <map>

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

template <class Kernel>
class LoopMapper {
  public:
    virtual void createKernels(
      const LoopTrees & loop_trees,
      std::set<std::list<Kernel *> > & kernel_lists
    ) const;
};

/** @} */

}

}

#ifndef __NO_TEMPLATE_INSTANTIATION__
#include "KLT/Core/loop-mapper.tpp"
#endif

#endif /* __KLT_CORE_LOOP_MAPPER_HPP__ */

