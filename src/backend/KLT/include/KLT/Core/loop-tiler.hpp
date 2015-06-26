
#ifndef __KLT_ITERATION_MAPPER_HPP__
#define __KLT_ITERATION_MAPPER_HPP__

namespace KLT {

template <class Annotation, class Language, class Runtime> class Kernel;

/*!
 * \addtogroup grp_klt_itmap
 * @{
*/

template <class Annotation, class Language, class Runtime>
class LoopTiler {
  public:
    struct loop_tiling_t {
      loop_tiling_t(typename LoopTrees<Annotation>::loop_t * loop_);

      typename LoopTrees<Annotation>::loop_t * loop;
      std::vector<typename Runtime::tile_desc_t> tiles;
    };


  protected:
    virtual void extendTiles(
      typename LoopTrees<Annotation>::loop_t * loop,
      loop_tiling_t * basic_tiling,
      std::vector<loop_tiling_t *> & extended_tiling
    ) const {
      extended_tiling.push_back(basic_tiling);
    }

  private:
    void determineTiles(
      Kernel<Annotation, Language, Runtime> * kernel,
      typename LoopTrees<Annotation>::node_t * node,
      std::map<typename LoopTrees<Annotation>::loop_t *, std::vector<loop_tiling_t *> > & tiling_map
    ) const {
      if (node == NULL) return;

      typename ::KLT::LoopTrees<Annotation>::loop_t  * loop  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::loop_t  *>(node);
      typename ::KLT::LoopTrees<Annotation>::cond_t  * cond  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::cond_t  *>(node);
      typename ::KLT::LoopTrees<Annotation>::block_t * block = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::block_t *>(node);

      if (loop != NULL) {
        std::vector<loop_tiling_t *> & tiling = tiling_map.insert(
          std::pair<typename LoopTrees<Annotation>::loop_t *, std::vector<loop_tiling_t *> >(
            loop, std::vector<loop_tiling_t *>()
          )
        ).first->second;

        tiling.push_back(new loop_tiling_t(loop));

        determineTiles(kernel, loop->block, tiling_map);
      }
      else if (cond != NULL) {
        determineTiles(kernel, cond->block_true, tiling_map);
        determineTiles(kernel, cond->block_false, tiling_map);
      }
      else if (block != NULL) {
        typename std::vector<typename LoopTrees<Annotation>::node_t *>::iterator it_child;
        for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
          determineTiles(kernel, *it_child, tiling_map);
      }
      else assert(dynamic_cast<typename ::KLT::LoopTrees<Annotation>::stmt_t *>(node) != NULL);
    }

  public:
    /** Create iteration mapping for a given loop distribution
     *  \param loop_distribution
     *  \param shapes
     */
    void determineTiles(
      Kernel<Annotation, Language, Runtime> * kernel,
      std::map<typename LoopTrees<Annotation>::loop_t *, std::vector<loop_tiling_t *> > & tiling_map
    ) const {
      const std::list<typename LoopTrees<Annotation>::node_t *> & roots = kernel->getRoots();
      typename std::list<typename LoopTrees<Annotation>::node_t *>::const_iterator it_root;
      for (it_root = roots.begin(); it_root != roots.end(); it_root++)
        determineTiles(kernel, *it_root, tiling_map);
    }
};

/** @} */

}

#endif /*  __KLT_ITERATION_MAPPER_HPP__ */

