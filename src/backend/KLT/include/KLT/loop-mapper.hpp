
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
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class Kernel;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation> class LoopTrees;

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
class LoopMapper {
  public:
    virtual void createKernels(
      const LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees,
      std::set<std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> > & kernel_lists
    ) const;
};

/** Very simple implementation of this method. It creates one Kernel per tree in the loop-trees.
 *  Then it select perfectly nested (fully parallel) loops for parallelisation.
 */
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
void LoopMapper<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::createKernels(
  const LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees,
  std::set<std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> > & kernel_lists
) const {
  const std::list< LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * > & trees = loop_trees.getTrees();
  std::list< LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * >::const_iterator it_tree;

  std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> kernel_list;

  for (it_tree = trees.begin(); it_tree != trees.end(); it_tree++) {
    LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * tree = *it_tree;

    // Create kernel for each loop-tree
    Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * kernel = new Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>(tree);
    kernel_list.push_back(kernel);

    // Create an empty loop mapping
    typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * loop_mapping = new typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t();
      kernel->getLoopMappings().insert(loop_mapping);

    // Collect all perfectly nested fully-parallel loops
    LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t * loop =
        dynamic_cast<LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t *>(tree);
    while (loop != NULL && loop->parallel_pattern == LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t::parfor) {
      loop_mapping->loop_nest.push_back(loop); // cond: loop != NULL && loop->parallel_pattern == LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::parfor
      if (loop->children.size() == 1) {
        tree = loop->children.front();
        loop = dynamic_cast<LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t *>(tree);
      }
      else break;
    }

    // Get the nodes that compose the body
    if (loop == NULL)
      loop_mapping->body.push_back(tree);
    else if (loop->parallel_pattern != LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t::parfor)
      loop_mapping->body.push_back(loop);
    else {
      assert(loop != NULL && loop->parallel_pattern == LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t::parfor);

      loop_mapping->body.insert(loop_mapping->body.end(), loop->children.begin(), loop->children.end());
    }
    
    assert(!loop_mapping->body.empty());
  }

  kernel_lists.insert(kernel_list);
}

/** @} */

}

#endif /* __KLT_LOOP_MAPPER_HPP__ */

