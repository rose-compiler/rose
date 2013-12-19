
#ifdef __NO_TEMPLATE_INSTANTIATION__
#error "Entering a template definition file when the macro __NO_TEMPLATE_INSTANTIATION__ is defined"
#endif

#include <cassert>

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

/** Very simple implementation of this method. It creates one Kernel per tree in the loop-trees.
 *  Then it select perfectly nested (fully parallel) loops for parallelisation.
 */
template <class Kernel>
void LoopMapper<Kernel>::createKernels(
  const LoopTrees & loop_trees,
  std::set<std::list<Kernel *> > & kernel_lists
) const {
  const std::list< LoopTrees::node_t * > & trees = loop_trees.getTrees();
  std::list< LoopTrees::node_t * >::const_iterator it_tree;

  std::list<Kernel *> kernel_list;

  for (it_tree = trees.begin(); it_tree != trees.end(); it_tree++) {
    LoopTrees::node_t * tree = *it_tree;

    // Create kernel for each loop-tree
    Kernel * kernel = new Kernel(tree);
    kernel_list.push_back(kernel);

    // Create an empty loop mapping
    typename Kernel::loop_mapping_t * loop_mapping = new typename Kernel::loop_mapping_t();
      kernel->getLoopMappings().insert(loop_mapping);

    // Collect all perfectly nested fully-parallel loops
    LoopTrees::loop_t * loop = dynamic_cast<LoopTrees::loop_t *>(tree);
    while (loop != NULL && loop->parallel_pattern == LoopTrees::loop_t::parfor) {
      loop_mapping->loop_nest.push_back(loop); // cond: loop != NULL && loop->parallel_pattern == LoopTrees::parfor
      if (loop->children.size() == 1) {
        tree = loop->children.front();
        loop = dynamic_cast<LoopTrees::loop_t *>(tree);
      }
      else break;
    }

    // Get the nodes that compose the body
    if (loop == NULL)
      loop_mapping->body.push_back(tree);
    else if (loop->parallel_pattern != LoopTrees::loop_t::parfor)
      loop_mapping->body.push_back(loop);
    else {
      assert(loop != NULL && loop->parallel_pattern == LoopTrees::loop_t::parfor);

      loop_mapping->body.insert(loop_mapping->body.end(), loop->children.begin(), loop->children.end());
    }
    
    assert(!loop_mapping->body.empty());
  }

  kernel_lists.insert(kernel_list);
}

/** @} */

}

}

