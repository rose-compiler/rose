
#ifndef __KLT_ITERATION_MAPPER_HPP__
#define __KLT_ITERATION_MAPPER_HPP__

namespace KLT {

template <class Annotation, class Language, class Runtime> class Kernel;

/*!
 * \addtogroup grp_klt_itmap
 * @{
*/

template <class Annotation, class Language, class Runtime>
class IterationMapper {
  protected:
    virtual void computeValidShapes(
      typename LoopTrees<Annotation>::loop_t * loop,
      std::vector<typename Runtime::loop_shape_t *> & shapes
    ) const = 0;

  private:
    void determineLoopShapes(
      Kernel<Annotation, Language, Runtime> * kernel,
      typename LoopTrees<Annotation>::node_t * node,
      std::map<typename LoopTrees<Annotation>::loop_t *, std::vector<typename Runtime::loop_shape_t *> > & shape_map
    ) const {
      if (node == NULL) return;

      typename ::KLT::LoopTrees<Annotation>::loop_t  * loop  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::loop_t  *>(node);
      typename ::KLT::LoopTrees<Annotation>::cond_t  * cond  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::cond_t  *>(node);
      typename ::KLT::LoopTrees<Annotation>::block_t * block = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::block_t *>(node);

      if (loop != NULL) {
        std::vector<typename Runtime::loop_shape_t *> & shapes = shape_map.insert(
          std::pair<typename LoopTrees<Annotation>::loop_t *, std::vector<typename Runtime::loop_shape_t *> >(
            loop, std::vector<typename Runtime::loop_shape_t *>()
          )
        ).first->second;

        computeValidShapes(loop, shapes);
      }
      else if (cond != NULL) {
        determineLoopShapes(kernel, cond->block_true, shape_map);
        determineLoopShapes(kernel, cond->block_false, shape_map);
      }
      else if (block != NULL) {
        typename std::vector<typename LoopTrees<Annotation>::node_t *>::iterator it_child;
        for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
          determineLoopShapes(kernel, *it_child, shape_map);
      }
    }

  public:
    /** Create iteration mapping for a given loop distribution
     *  \param loop_distribution
     *  \param shapes
     */
    void determineLoopShapes(
      Kernel<Annotation, Language, Runtime> * kernel,
      std::map<typename LoopTrees<Annotation>::loop_t *, std::vector<typename Runtime::loop_shape_t *> > & shape_map
    ) const {
      const std::list<typename LoopTrees<Annotation>::node_t *> & roots = kernel->getRoots();
      typename std::list<typename LoopTrees<Annotation>::node_t *>::const_iterator it_root;
      for (it_root = roots.begin(); it_root != roots.end(); it_root++) {
        determineLoopShapes(kernel, *it_root, shape_map);
      }
    }
};

/** @} */

}

#endif /*  __KLT_ITERATION_MAPPER_HPP__ */

