
#ifndef __KLT_ITERATION_MAPPER_HPP__
#define __KLT_ITERATION_MAPPER_HPP__

namespace KLT {

template <class Annotation, class Runtime> class Kernel;

/*!
 * \addtogroup grp_klt_itmap
 * @{
*/

template <class Annotation, class Runtime>
class LoopTiler {
  public:
//  typedef Kernel<Annotation, Runtime> Kernel;
//  typedef LoopTrees<Annotation> LoopTrees;
    typedef typename LoopTrees<Annotation>::node_t node_t;
    typedef typename LoopTrees<Annotation>::loop_t loop_t;
    typedef typename LoopTrees<Annotation>::cond_t cond_t;
    typedef typename LoopTrees<Annotation>::block_t block_t;
    typedef typename LoopTrees<Annotation>::stmt_t stmt_t;

    struct loop_tiling_t {
      loop_t * loop;
      std::vector<typename Runtime::tile_desc_t> tiles;

      loop_tiling_t(loop_t * loop_);
    };


  protected:
    virtual void extendTiles(loop_t * loop, loop_tiling_t * basic_tiling, std::vector<loop_tiling_t *> & extended_tiling) const;

  private:
    void determineTiles(Kernel<Annotation, Runtime> * kernel, node_t * node, std::map<loop_t *, std::vector<loop_tiling_t *> > & tiling_map) const;

  public:
    /** Create iteration mapping for a given loop distribution
     *  \param loop_distribution
     *  \param shapes
     */
    void determineTiles(Kernel<Annotation, Runtime> * kernel, std::map<loop_t *, std::vector<loop_tiling_t *> > & tiling_map) const;
};

template <class Annotation, class Runtime>
LoopTiler<Annotation, Runtime>::loop_tiling_t::loop_tiling_t(loop_t * loop_) :
  loop(loop_), tiles()
{
  typename std::vector<Annotation>::const_iterator it_annotation;
  for (it_annotation = loop->annotations.begin(); it_annotation != loop->annotations.end(); it_annotation++) {
    typename Annotation::Language::tile_clause_t * tile_clause = Annotation::Language::isTileClause(it_annotation->clause);
    if (tile_clause != NULL) {
      tiles.push_back(typename Runtime::tile_desc_t());
      tiles.back().init(tile_clause);
    }
  }
}

template <class Annotation, class Runtime>
void LoopTiler<Annotation, Runtime>::extendTiles(
  loop_t * loop, loop_tiling_t * basic_tiling,
  std::vector<loop_tiling_t *> & extended_tiling
) const {
  extended_tiling.push_back(basic_tiling);
}

template <class Annotation, class Runtime>
void LoopTiler<Annotation, Runtime>::determineTiles(Kernel<Annotation, Runtime> * kernel, node_t * node, std::map<loop_t *, std::vector<loop_tiling_t *> > & tiling_map) const {
  if (node == NULL) return;

  loop_t  * loop  = dynamic_cast<loop_t  *>(node);
  cond_t  * cond  = dynamic_cast<cond_t  *>(node);
  block_t * block = dynamic_cast<block_t *>(node);

  if (loop != NULL) {
    std::vector<loop_tiling_t *> & tiling = tiling_map.insert(
      std::pair<loop_t *, std::vector<loop_tiling_t *> >(loop, std::vector<loop_tiling_t *>())
    ).first->second;

    tiling.push_back(new loop_tiling_t(loop));

    determineTiles(kernel, loop->block, tiling_map);
  }
  else if (cond != NULL) {
    determineTiles(kernel, cond->block_true, tiling_map);
    determineTiles(kernel, cond->block_false, tiling_map);
  }
  else if (block != NULL) {
    typename std::vector<node_t *>::iterator it_child;
    for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
      determineTiles(kernel, *it_child, tiling_map);
  }
  else assert(dynamic_cast<stmt_t *>(node) != NULL);
}

template <class Annotation, class Runtime>
void LoopTiler<Annotation, Runtime>::determineTiles(Kernel<Annotation, Runtime> * kernel, std::map<loop_t *, std::vector<loop_tiling_t *> > & tiling_map) const {
  const std::list<node_t *> & roots = kernel->getRoots();
  typename std::list<node_t *>::const_iterator it_root;
  for (it_root = roots.begin(); it_root != roots.end(); it_root++)
    determineTiles(kernel, *it_root, tiling_map);
}

/** @} */

}

#endif /*  __KLT_ITERATION_MAPPER_HPP__ */

