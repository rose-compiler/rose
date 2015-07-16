
#ifndef __KLT_LOOP_MAPPER_HPP__
#define __KLT_LOOP_MAPPER_HPP__

#include <list>
#include <map>

namespace KLT {

/*!
 * \addtogroup grp_klt_loopmap
 * @{
*/

template <class Annotation> class LoopTrees;
template <class Annotation, class Runtime> class Kernel;

template <class Annotation, class Runtime>
class LoopMapper {
  public:
    typedef typename LoopTrees<Annotation>::node_t node_t;

    virtual void createKernels(const LoopTrees<Annotation> & loop_trees, std::set<std::list<Kernel<Annotation, Runtime> *> > & kernel_lists) const;
};

/** 
 *  Default imlementation, it creates only one kernel.
 *  More advanced version could distribute the LoopTree into multiple kernels that could be executed in parallel
 */
template <class Annotation, class Runtime>
void LoopMapper<Annotation, Runtime>::createKernels(const LoopTrees<Annotation> & loop_trees, std::set<std::list<Kernel<Annotation, Runtime> *> > & kernel_lists) const {
  std::list<Kernel<Annotation, Runtime> *> kernel_list;

  Kernel<Annotation, Runtime> * kernel = new Kernel<Annotation, Runtime>(loop_trees);
  kernel_list.push_back(kernel);

  const std::vector<node_t * > & trees = loop_trees.getTrees();
  typename std::vector<node_t * >::const_iterator it;
  for (it = trees.begin(); it != trees.end(); it++)
    kernel->appendRoot(*it);

  kernel_lists.insert(kernel_list);
}

/** @} */

}

#endif /* __KLT_LOOP_MAPPER_HPP__ */

