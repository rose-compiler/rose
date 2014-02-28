
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
    ) const;

  private:
    void determineLoopShapes(
      Kernel<Annotation, Language, Runtime> * kernel,
      typename LoopTrees<Annotation>::loop_t * loop,
      std::map<typename LoopTrees<Annotation>::loop_t *, std::vector<typename Runtime::loop_shape_t *> > & shape_map
    ) const {
      std::vector<typename Runtime::loop_shape_t *> & shapes = shape_map.insert(
        std::pair<typename LoopTrees<Annotation>::loop_t *, std::vector<typename Runtime::loop_shape_t *> >(
          loop, std::vector<typename Runtime::loop_shape_t *>()
        )
      ).first->second;

      computeValidShapes(loop, shapes);

      if (shapes.empty()) shape_map.erase(loop);

      typename std::list<typename LoopTrees<Annotation>::node_t *>::iterator it_child;
      for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++) {
        typename LoopTrees<Annotation>::loop_t * child = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(*it_child);
        if (child != NULL) determineLoopShapes(kernel, child, shape_map);
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
        typename LoopTrees<Annotation>::loop_t * loop = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(*it_root);
        if (loop != NULL) determineLoopShapes(kernel, loop, shape_map);
      }
    }
};

/** @} */

}

#endif /*  __KLT_ITERATION_MAPPER_HPP__ */

