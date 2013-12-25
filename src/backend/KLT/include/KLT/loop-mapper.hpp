
#ifndef __KLT_LOOP_MAPPER_HPP__
#define __KLT_LOOP_MAPPER_HPP__

#include <list>
#include <map>

namespace KLT {

/*!
 * \addtogroup grp_klt_loopmap
 * @{
*/

template <class Annotation> class Data;
template <class Annotation, class Language, class Runtime> class Kernel;
template <class Annotation> class LoopTrees;

template <class Annotation, class Language, class Runtime>
class LoopMapper {
  public:
    virtual void createKernels(
      const LoopTrees<Annotation> & loop_trees,
      std::set<std::list<Kernel<Annotation, Language, Runtime> *> > & kernel_lists
    ) const;
};

/** Very simple implementation of this method. It creates one Kernel per tree in the loop-trees.
 *  Then it select perfectly nested (fully parallel) loops for parallelisation.
 */
template <class Annotation, class Language, class Runtime>
void LoopMapper<Annotation, Language, Runtime>::createKernels(
  const LoopTrees<Annotation> & loop_trees,
  std::set<std::list<Kernel<Annotation, Language, Runtime> *> > & kernel_lists
) const {
  const std::list<typename LoopTrees<Annotation>::node_t * > & trees = loop_trees.getTrees();
  typename std::list<typename LoopTrees<Annotation>::node_t * >::const_iterator it_tree;

  std::list<Kernel<Annotation, Language, Runtime> *> kernel_list;

  for (it_tree = trees.begin(); it_tree != trees.end(); it_tree++) {
    typename LoopTrees<Annotation>::node_t * tree = *it_tree;

    // Create kernel for each loop-tree
    Kernel<Annotation, Language, Runtime> * kernel = new Kernel<Annotation, Language, Runtime>(tree);
    kernel_list.push_back(kernel);

    // Create an empty loop mapping
    typename Kernel<Annotation, Language, Runtime>::loop_mapping_t * loop_mapping = new typename Kernel<Annotation, Language, Runtime>::loop_mapping_t();
      kernel->getLoopMappings().insert(loop_mapping);

    // Collect all perfectly nested parallel loops
    typename LoopTrees<Annotation>::loop_t * loop =
        dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(tree);
    while (loop != NULL && loop->isParallel()) {
      loop_mapping->loop_nest.push_back(loop);
      if (loop->children.size() == 1) {
        tree = loop->children.front();
        loop = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(tree);
      }
      else break;
    }

    // Get the nodes that compose the body
    if (loop == NULL)
      loop_mapping->body.push_back(tree);
    else if (!loop->isParallel())
      loop_mapping->body.push_back(loop);
    else {
      assert(loop != NULL && loop->isParallel());

      loop_mapping->body.insert(loop_mapping->body.end(), loop->children.begin(), loop->children.end());
    }
    
    assert(!loop_mapping->body.empty());
  }

  kernel_lists.insert(kernel_list);
}

/** @} */

}

#endif /* __KLT_LOOP_MAPPER_HPP__ */

