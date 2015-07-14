
#ifndef __KLT_MFB_KLT_HPP__
#define __KLT_MFB_KLT_HPP__

#include "KLT/Core/kernel.hpp"
#include "KLT/Core/runtime.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/function-declaration.hpp"

namespace MFB {

/*!
 * \addtogroup grp_klt_mfb
 * @{
*/

template <class Object>
class KLT {};

template <>
class Driver<KLT> : public Driver<Sage> {
  protected:
    bool guard_kernel_decl;

  public:
    Driver(SgProject * project_, bool guard_kernel_decl_ = false);
    virtual ~Driver();

    template <class Object>
    typename KLT<Object>::build_result_t build(typename KLT<Object>::object_desc_t const & object);
};

template <class Annotation, class Language, class Runtime>
SgBasicBlock * createLocalDeclarations(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  typename ::KLT::Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
  const typename ::KLT::Kernel<Annotation, Language, Runtime>::arguments_t & arguments,
  const std::map<
    typename ::KLT::LoopTrees<Annotation>::loop_t *,
    typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *
  > & loop_tiling
);

template <class Annotation>
bool compareTiles(typename ::KLT::LoopTrees<Annotation>::tile_t * t1, typename ::KLT::LoopTrees<Annotation>::tile_t * t2) {
  return t1->order < t2->order;
}

template <class Annotation, class Language, class Runtime>
typename ::KLT::LoopTrees<Annotation>::node_t * collapseLoopsAndTiles(
  typename ::KLT::LoopTrees<Annotation>::node_t * node,
  const std::map<typename ::KLT::LoopTrees<Annotation>::loop_t *, typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *> & loop_tiling,
  std::vector<typename Runtime::loop_desc_t *> & loops,
  std::vector<typename Runtime::tile_desc_t *> & tiles
) {
  typedef typename ::KLT::LoopTrees<Annotation>::node_t  node_t;
  typedef typename ::KLT::LoopTrees<Annotation>::loop_t  loop_t;
  typedef typename ::KLT::LoopTrees<Annotation>::tile_t  tile_t;
  typedef typename ::KLT::LoopTrees<Annotation>::cond_t  cond_t;
  typedef typename ::KLT::LoopTrees<Annotation>::block_t block_t;
  typedef typename ::KLT::LoopTrees<Annotation>::stmt_t  stmt_t;

  typedef typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t loop_tiling_t;

  loop_t  * loop  = dynamic_cast<loop_t  *>(node);
  tile_t  * tile  = dynamic_cast<tile_t  *>(node);
  cond_t  * cond  = dynamic_cast<cond_t  *>(node);
  block_t * block = dynamic_cast<block_t *>(node);
  stmt_t  * stmt  = dynamic_cast<stmt_t  *>(node);
  if (loop != NULL) {
    typename std::map<loop_t *, loop_tiling_t *>::const_iterator it_tiling = loop_tiling.find(loop);
    if (it_tiling != loop_tiling.end() && it_tiling->second != NULL && it_tiling->second->tiles.size() > 0) {
      
      std::map<typename ::KLT::LoopTrees<Annotation>::loop_t *, typename Runtime::loop_desc_t *> loop_map;
      std::vector<std::pair<loop_t *, loop_tiling_t *> > collapsable_loops;
      block_t * next_block = NULL;
      while (loop != NULL && it_tiling != loop_tiling.end() && it_tiling->second != NULL && it_tiling->second->tiles.size() > 0) {
//      std::cerr << "# A >> Loop " << loop->id << std::endl;
        typename Runtime::loop_desc_t * loop_desc = new typename Runtime::loop_desc_t(loop->id, loop->lower_bound, loop->upper_bound, loop->stride);
        loops.push_back(loop_desc);
        loop_map.insert(std::pair<typename ::KLT::LoopTrees<Annotation>::loop_t *, typename Runtime::loop_desc_t *>(loop, loop_desc));

        collapsable_loops.push_back(
          std::pair<loop_t *, loop_tiling_t *>(loop, it_tiling->second)
        );

        assert(loop->block != NULL);

        next_block = loop->block;
        if (next_block->children.size() == 1) {
          loop = dynamic_cast<loop_t  *>(next_block->children[0]);
          if (loop != NULL) it_tiling = loop_tiling.find(loop);
        }
        else loop = NULL;
      }
      assert(collapsable_loops.size() > 0);

      std::vector<tile_t *> collapsed_tiles;
      typename std::vector<std::pair<loop_t *, loop_tiling_t *> >::const_iterator it_collapsable;
      for (it_collapsable = collapsable_loops.begin(); it_collapsable != collapsable_loops.end(); it_collapsable++) {
        assert(it_collapsable->first == it_collapsable->second->loop);

        typename std::vector<typename Runtime::tile_desc_t>::iterator it_tile;
        for (it_tile = it_collapsable->second->tiles.begin(); it_tile != it_collapsable->second->tiles.end(); it_tile++) {
          it_tile->id = tiles.size();
          tile_t * tile = new tile_t(it_tile->id, it_tile->order, it_tile->iterator_sym, it_collapsable->first);
          collapsed_tiles.push_back(tile);

//        std::cerr << "# B >> Tile " << it_tile->id << std::endl;
          typename Runtime::tile_desc_t * tile_desc = new typename Runtime::tile_desc_t(*it_tile);
          tiles.push_back(tile_desc);
          typename std::map<typename ::KLT::LoopTrees<Annotation>::loop_t *, typename Runtime::loop_desc_t *>::const_iterator it_loop_map = loop_map.find(it_collapsable->first);
          assert(it_loop_map != loop_map.end());
          it_loop_map->second->tiles.push_back(tile_desc);
        }
      }

      std::sort(collapsed_tiles.begin(), collapsed_tiles.end(), &compareTiles<Annotation>);

      typename std::vector<tile_t *>::const_iterator it_collapsed;
      for (it_collapsed = collapsed_tiles.begin(); it_collapsed != collapsed_tiles.end() - 1; it_collapsed++)
        (*it_collapsed)->tile = *(it_collapsed + 1);
      collapsed_tiles.back()->block = (block_t *)collapseLoopsAndTiles<Annotation, Language, Runtime>(next_block, loop_tiling, loops, tiles);

      return collapsed_tiles.front();
    }
    else {
//    std::cerr << "# A >> Loop " << loop->id << std::endl;
      loops.push_back(new typename Runtime::loop_desc_t(loop->id, loop->lower_bound, loop->upper_bound, loop->stride));

      loop_t * new_loop = new loop_t(loop->id, loop->iterator, loop->lower_bound, loop->upper_bound, loop->stride);
        new_loop->annotations = loop->annotations;
        new_loop->block = (block_t *)collapseLoopsAndTiles<Annotation, Language, Runtime>(loop->block, loop_tiling, loops, tiles);
      return new_loop;
    }
  }
  else if (tile != NULL) {
    assert(false);
  }
  else if (cond != NULL) {
    cond_t * new_cond = new cond_t(cond->condition);
      new_cond->block_true  = (block_t *)collapseLoopsAndTiles<Annotation, Language, Runtime>(cond->block_true , loop_tiling, loops, tiles);
      new_cond->block_false = (block_t *)collapseLoopsAndTiles<Annotation, Language, Runtime>(cond->block_false, loop_tiling, loops, tiles);
    return new_cond;
  }
  else if (block != NULL) {
    typename std::vector<node_t *>::const_iterator it_child;
    block_t * new_block = new block_t();
    for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
      new_block->children.push_back(collapseLoopsAndTiles<Annotation, Language, Runtime>(*it_child, loop_tiling, loops, tiles));
    return new_block;
  }
  else if (stmt != NULL) {
    return new stmt_t(stmt->statement);
  }
  else assert(false);
}

template <class Object>
typename KLT<Object>::build_result_t Driver<KLT>::build(typename KLT<Object>::object_desc_t const & object) {
  typedef typename KLT<Object>::Annotation Annotation;
  typedef typename KLT<Object>::Language Language;
  typedef typename KLT<Object>::Runtime Runtime;

  typedef typename ::KLT::LoopTrees<Annotation>::node_t node_t;
  typedef typename ::KLT::LoopTrees<Annotation>::loop_t loop_t;

  std::ostringstream kernel_function_name;
  kernel_function_name << "kernel_" << object.kernel << "_" << object.id;
  typename KLT<Object>::build_result_t result = new typename ::KLT::Kernel<Annotation, Language, Runtime>::kernel_desc_t(kernel_function_name.str());

  std::map<SgVariableSymbol *, SgVariableSymbol *> param_to_field_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *> coef_to_field_map;
  std::map< ::KLT::Data<Annotation> *, SgVariableSymbol *> data_to_field_map;

  typename std::list<loop_t *>::const_iterator it_nested_loop;
  typename std::list<node_t *>::const_iterator it_body_branch;

  // * Function Declaration *

  SgFunctionParameterList * kernel_function_params = ::KLT::createParameterList<Annotation, Language, Runtime>(object.kernel);

  ::KLT::Runtime::get_exec_config<Annotation, Language, Runtime>(result->config, object.kernel);

  MFB::Sage<SgFunctionDeclaration>::object_desc_t kernel_function_desc(
    result->kernel_name,
    SageBuilder::buildVoidType(),
    kernel_function_params,
    NULL,
    object.file_id,
    object.file_id
  );

//Driver<Sage> * sage_driver = dynamic_cast<Driver<Sage> *>(this);
  Driver<Sage> * sage_driver = (Driver<Sage> *)this;
  assert(sage_driver != NULL);

  MFB::Sage<SgFunctionDeclaration>::build_result_t kernel_result = 
    sage_driver->build<SgFunctionDeclaration>(kernel_function_desc);

  {
    SgFunctionDeclaration * kernel_decl = kernel_result.symbol->get_declaration();
    assert(kernel_decl != NULL);

    SgFunctionDeclaration * first_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_firstNondefiningDeclaration());
    assert(first_kernel_decl != NULL);
    KLT<Object>::Language::applyKernelModifiers(first_kernel_decl);

    SgFunctionDeclaration * defn_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_definingDeclaration());
    assert(defn_kernel_decl != NULL);
    KLT<Object>::Language::applyKernelModifiers(defn_kernel_decl);

    if (guard_kernel_decl)
      SageInterface::guardNode(defn_kernel_decl, std::string("defined(ENABLE_") + result->kernel_name + ")");
  }

  // * Local Declarations *

  typename ::KLT::Kernel<Annotation, Language, Runtime>::local_symbol_maps_t local_symbol_maps;

  SgBasicBlock * body = createLocalDeclarations<Annotation, Language, Runtime> (
    *(Driver<Sage> *)this,
    kernel_result.definition,
    local_symbol_maps,
    object.kernel->getArguments(),
    object.tiling
  );

  const std::list<node_t *> & kernel_roots = object.kernel->getRoots();
  typename std::list<node_t *>::const_iterator it_root;
  for (it_root = kernel_roots.begin(); it_root != kernel_roots.end(); it_root++) {
    node_t * root = *it_root;

//  ::KLT::LoopTrees<Annotation>::toText(root, std::cerr, "# 1 >> "); std::cerr << std::endl;

    root = collapseLoopsAndTiles<typename KLT<Object>::Annotation, typename KLT<Object>::Language, typename KLT<Object>::Runtime>(*it_root, object.tiling, result->loops, result->tiles);

//  ::KLT::LoopTrees<Annotation>::toText(root, std::cerr, "# 2 >> "); std::cerr << std::endl;

    ::KLT::generateKernelBody<Annotation, Language, Runtime>(root, (typename Runtime::exec_mode_t)0, result->config, local_symbol_maps, body);
  }

//std::cerr << "# >> Kernel " << result->kernel_name << " (" << result->id << ")" << std::endl;

//typename std::vector<typename Runtime::loop_desc_t *>::const_iterator it_loop;
//for (it_loop = result->loops.begin(); it_loop != result->loops.end(); it_loop++) {
//  std::cerr << "# >> Loop " << (*it_loop)->id << std::endl;
//}

//typename std::vector<typename Runtime::tile_desc_t *>::const_iterator it_tile;
//for (it_tile = result->tiles.begin(); it_tile != result->tiles.end(); it_tile++) {
//  std::cerr << "# >> Tile " << (*it_tile)->id << std::endl;
//}

  SageInterface::setSourcePositionForTransformation(body);

  return result;
}

/** @} */

}

#endif /* __KLT__MFB_KLT_HPP__ */
