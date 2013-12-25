
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
  private:
    typename Runtime::loop_shape_t * createShape(typename LoopTrees<Annotation>::loop_t * loop) const;

    void determineShapes(Kernel<Annotation, Language, Runtime> * kernel, typename LoopTrees<Annotation>::loop_t * loop) const {
      kernel->setShape(loop, createShape(loop));
      typename std::list<typename LoopTrees<Annotation>::node_t *>::iterator it_child;
      for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++) {
        typename LoopTrees<Annotation>::loop_t * child = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(*it_child);
        if (child != NULL) determineShapes(kernel, child);
      }
    }

  public:
    /** Create iteration mapping for a given loop distribution
     *  \param loop_distribution
     *  \param shapes
     */
    void determineShapes(Kernel<Annotation, Language, Runtime> * kernel) const {
      const std::list<typename LoopTrees<Annotation>::node_t *> & roots = kernel->getRoots();
      typename std::list<typename LoopTrees<Annotation>::node_t *>::const_iterator it_root;
      for (it_root = roots.begin(); it_root != roots.end(); it_root++) {
        typename LoopTrees<Annotation>::loop_t * loop = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(*it_root);
        if (loop != NULL) determineShapes(kernel, loop);
      }
    }
};

/** @} */

}

#endif /*  __KLT_ITERATION_MAPPER_HPP__ */

