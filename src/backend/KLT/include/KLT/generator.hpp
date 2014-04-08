
#ifndef __KLT_GENERATOR_HPP__
#define __KLT_GENERATOR_HPP__

#include <map>
#include <set>
#include <list>
#include <string>
#include <iostream>

#include "KLT/mfb-klt.hpp"

class SgProject;
class SgSourceFile;
class SgVariableSymbol;

namespace KLT {

template <class Annotation> class Data;
template <class Annotation, class Language, class Runtime> class Kernel;
template <class Annotation> class LoopTrees;

template <class Annotation, class Language, class Runtime> class LoopMapper;
template <class Annotation, class Language, class Runtime> class IterationMapper;
template <class Annotation, class Language, class Runtime> class DataFlow;

template <class Annotation, class Language, class Runtime> class IterationMap;

template <class Annotation, class Language, class Runtime> class CG_Config;

/*!
 * \addtogroup grp_klt_codegen
 * @{
*/

template <class Annotation, class Language, class Runtime, class Driver = ::MFB::KLT_Driver>
class Generator {
  protected:
    Driver & p_driver;

    ::MFB::Driver< ::MFB::KLT  > & p_klt_driver;
    ::MFB::Driver< ::MFB::Sage > & p_sage_driver;

    std::string p_file_name;
    unsigned long  p_file_id;

  protected:
    unsigned long createFile();

    void buildArgumentLists(const LoopTrees<Annotation> & loop_trees, Kernel<Annotation, Language, Runtime> * kernel);

    void callToKernelBuilder(
      Kernel<Annotation, Language, Runtime> * kernel
    ) const;

  public:
    Generator(Driver & driver, const std::string & file_name);
    virtual ~Generator();

    void generate(
      const LoopTrees<Annotation> & loop_trees,
      std::set<std::list<Kernel<Annotation, Language, Runtime> *> > & kernel_lists,
      const CG_Config<Annotation, Language, Runtime> & cg_config
    );
};

template <class Annotation>
bool cmpDataName(const Data<Annotation> * data_lhs, const Data<Annotation> * data_rhs) {
  return data_lhs->getVariableSymbol()->get_name().getString() < data_rhs->getVariableSymbol()->get_name().getString();
}

template <class Annotation, class Language, class Runtime, class Driver>
void Generator<Annotation, Language, Runtime, Driver>::buildArgumentLists(
  const LoopTrees<Annotation> & loop_trees,
  Kernel<Annotation, Language, Runtime> * kernel
) {
  std::set<SgVariableSymbol *>::const_iterator it_symbol;
  typename std::set<Data<Annotation> *>::const_iterator it_data;

  const std::set<SgVariableSymbol *> & parameters = loop_trees.getParameters();
  const std::set<SgVariableSymbol *> & scalars = loop_trees.getScalars();

  std::set<SgVariableSymbol *> sym_var_refs;
    collectReferencedSymbols(kernel, sym_var_refs);

  for (it_symbol = parameters.begin(); it_symbol != parameters.end(); it_symbol++) {
    SgVariableSymbol * parameter = *it_symbol;

    bool used = sym_var_refs.find(parameter) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().parameters.push_back(parameter);
  }

  for (it_symbol = scalars.begin(); it_symbol != scalars.end(); it_symbol++) {
    SgVariableSymbol * scalar = *it_symbol;

    bool used = sym_var_refs.find(scalar) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().scalars.push_back(scalar);
  }

  std::vector<Data<Annotation> *> sorted_data;
  for (it_data = kernel->getDataflow().datas.begin(); it_data != kernel->getDataflow().datas.end(); it_data++) {
    Data<Annotation> * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol(); // FIXME Whole data approx (same symbol same data)

    bool used = sym_var_refs.find(data_sym) != sym_var_refs.end();

    if (used) sorted_data.push_back(data); // If it is referenced (SgVarRefExp) then it is needed
  }

  std::sort(sorted_data.begin(), sorted_data.end(), &cmpDataName<Annotation>);
  kernel->getArguments().datas.insert(kernel->getArguments().datas.end(), sorted_data.begin(), sorted_data.end());

  // FIXME with partial data we might end up referencing multiple time the same variable..
}

template <class Annotation, class Language, class Runtime, class Driver>
Generator<Annotation, Language, Runtime, Driver>::Generator(
  Driver & driver,
  const std::string & file_name
) :
  p_driver(driver),
  p_klt_driver(p_driver),
  p_sage_driver(p_driver),
  p_file_name(file_name),
  p_file_id(createFile())
{}

template <class Annotation, class Language, class Runtime, class Driver>
Generator<Annotation, Language, Runtime, Driver>::~Generator() {}

/// Helper function to build set of all loop shape configuration.
template <class Annotation, class Language, class Runtime>
void buildAllShapeConfigs(
  std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> curr_elem,
  typename std::map<typename LoopTrees<Annotation>::loop_t *,  std::vector<typename Runtime::loop_shape_t *> >::const_iterator curr_it,
  const typename std::map<typename LoopTrees<Annotation>::loop_t *,  std::vector<typename Runtime::loop_shape_t *> >::const_iterator end_it,
  std::set<std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> > & shape_set
) {
  if (curr_it == end_it) {
    shape_set.insert(curr_elem);
  }
  else {
    typename std::map<typename LoopTrees<Annotation>::loop_t *,  std::vector<typename Runtime::loop_shape_t *> >::const_iterator new_curr_it = curr_it;
    new_curr_it++;

    if (curr_it->second.empty()) {
      curr_elem.insert(std::pair<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *>(curr_it->first, NULL));
      buildAllShapeConfigs<Annotation, Language, Runtime>(curr_elem, new_curr_it, end_it, shape_set);
    }
    else {
      typename std::vector<typename Runtime::loop_shape_t *>::const_iterator it;
      for (it = curr_it->second.begin(); it != curr_it->second.end(); it++) {
        std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> new_curr_elem(curr_elem);
        new_curr_elem.insert(std::pair<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *>(curr_it->first, *it));
        buildAllShapeConfigs<Annotation, Language, Runtime>(new_curr_elem, new_curr_it, end_it, shape_set);
      }
    }
  }
}

template <class Annotation, class Language, class Runtime>
unsigned getUnrollingFactor(
  typename LoopTrees<Annotation>::loop_t * loop,
  const std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> & shaping
) {
  unsigned child_unrolling_factor = 1;
  typename std::list<typename LoopTrees<Annotation>::node_t *>::const_iterator it_child;
  for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++) {
    typename LoopTrees<Annotation>::loop_t * child_loop = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(*it_child);
    if (child_loop != NULL) {
      unsigned tmp_unrolling_factor = getUnrollingFactor<Annotation, Language, Runtime>(child_loop, shaping);
      child_unrolling_factor = child_unrolling_factor > tmp_unrolling_factor ? child_unrolling_factor : tmp_unrolling_factor;
    }
  }

  unsigned unrolling_factor = 1;
  typename std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *>::const_iterator it_shape = shaping.find(loop);
  assert(it_shape != shaping.end());
  if (it_shape->second != NULL) {
    if (it_shape->second->unroll_tile_0)
      unrolling_factor = unrolling_factor > it_shape->second->tile_0 ? unrolling_factor : it_shape->second->tile_0;
    if (it_shape->second->unroll_tile_1)
      unrolling_factor = unrolling_factor > it_shape->second->tile_1 ? unrolling_factor : it_shape->second->tile_1;
    if (it_shape->second->unroll_tile_2)
      unrolling_factor = unrolling_factor > it_shape->second->tile_2 ? unrolling_factor : it_shape->second->tile_2;
    if (it_shape->second->unroll_tile_3)
      unrolling_factor = unrolling_factor > it_shape->second->tile_3 ? unrolling_factor : it_shape->second->tile_3;
  }

  return child_unrolling_factor * unrolling_factor;
}

template <class Annotation, class Language, class Runtime>
unsigned getUnrollingFactor(
  Kernel<Annotation, Language, Runtime> * kernel,
  const std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> & shaping
) {
  unsigned unrolling_factor = 1;
  const std::list<typename LoopTrees<Annotation>::node_t *> & roots = kernel->getRoots();
  typename std::list<typename LoopTrees<Annotation>::node_t *>::const_iterator it_root;
  for (it_root = roots.begin(); it_root != roots.end(); it_root++) {
    typename LoopTrees<Annotation>::loop_t * loop = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(*it_root);
    if (loop != NULL) {
      unsigned tmp_unrolling_factor = getUnrollingFactor<Annotation, Language, Runtime>(loop, shaping);
      unrolling_factor = unrolling_factor > tmp_unrolling_factor ? unrolling_factor : tmp_unrolling_factor;
    }
  }
  return unrolling_factor;
}

template <class Annotation, class Language, class Runtime, class Driver>
void Generator<Annotation, Language, Runtime, Driver>::generate(
  const LoopTrees<Annotation> & loop_trees,
  std::set<std::list<Kernel<Annotation, Language, Runtime> *> > & kernel_lists,
  const CG_Config<Annotation, Language, Runtime> & cg_config
) {
  typename std::set<std::list<Kernel<Annotation, Language, Runtime> *> >::const_iterator it_kernel_list;
  typename std::list<Kernel<Annotation, Language, Runtime> *>::const_iterator it_kernel;
  typename std::set<IterationMap<Annotation, Language, Runtime> *>::const_iterator it_iteration_map;

  // 1 - Loop Selection : Generate multiple list of kernel that implement the given LoopTree

  cg_config.getLoopMapper().createKernels(loop_trees, kernel_lists);

  // 2 - Data Flow : performs data-flow analysis for each list of kernel

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++)
    cg_config.getDataFlow().generateFlowSets(loop_trees, *it_kernel_list);

  // 3 - Arguments : determines the list of arguments needed by each kernel

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++)
    for (it_kernel = it_kernel_list->begin(); it_kernel != it_kernel_list->end(); it_kernel++)
      buildArgumentLists(loop_trees, *it_kernel);

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++)
    for (it_kernel = it_kernel_list->begin(); it_kernel != it_kernel_list->end(); it_kernel++) {

      // 4 - Iterations Mapping : determines the "shape" of every loop of each kernel.
      //     The "shape" of a loop is how this loop is adapted to the execution model.

      std::map<typename LoopTrees<Annotation>::loop_t *, std::vector<typename Runtime::loop_shape_t *> > shape_map;
      typename std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *>::const_iterator it_loop_shape;
      typename std::map<typename LoopTrees<Annotation>::loop_t *, std::vector<typename Runtime::loop_shape_t *> >::const_iterator it_shape_vect;
      typename std::vector<typename Runtime::loop_shape_t *>::const_iterator it_shape;

      cg_config.getIterationMapper().determineLoopShapes(*it_kernel, shape_map);

      std::set<std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> > loop_shape_set;
      buildAllShapeConfigs<Annotation, Language, Runtime>(
        std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *>(),
        shape_map.begin(),
        shape_map.end(),
        loop_shape_set
      );

      // 5 - Code Generation

      unsigned cnt = 0;
      typename std::set<std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> >::iterator it_loop_shape_map;
      for (it_loop_shape_map = loop_shape_set.begin(); it_loop_shape_map != loop_shape_set.end(); it_loop_shape_map++) {

        unsigned unrolling_factor = getUnrollingFactor<Annotation, Language, Runtime>(*it_kernel, *it_loop_shape_map);

        std::cerr << "Generate kernel version " << cnt << "/" << loop_shape_set.size() << std::endl;
        for (it_loop_shape = it_loop_shape_map->begin(); it_loop_shape != it_loop_shape_map->end(); it_loop_shape++) {
          std::cerr << "  Loop: " << it_loop_shape->first->iterator->get_name().getString() << std::endl;
          if (it_loop_shape->second != NULL) {
            std::cerr << "      Tile 0: " << it_loop_shape->second->tile_0 << " " << it_loop_shape->second->unroll_tile_0 << std::endl;
            std::cerr << "      Tile 1: " << it_loop_shape->second->tile_1 << " " << it_loop_shape->second->unroll_tile_1 << std::endl;
            std::cerr << "      Tile 2: " << it_loop_shape->second->tile_2 << " " << it_loop_shape->second->unroll_tile_2 << std::endl;
            std::cerr << "      Tile 3: " << it_loop_shape->second->tile_3 << " " << it_loop_shape->second->unroll_tile_3 << std::endl;
          }
          else
            std::cerr << "      No shape information." << std::endl;
        }
        std::cerr << "    Unrolling Factor = " << unrolling_factor << std::endl;

        if (unrolling_factor > 128) {
          std::cerr << "  Skip this shape configuration because of a Unrolling factor too large." << std::endl;
          continue;
        }

        typename ::MFB::KLT<Kernel<Annotation, Language, Runtime> >::object_desc_t kernel_desc(cnt++, *it_kernel, p_file_id);

        kernel_desc.shapes.insert(it_loop_shape_map->begin(), it_loop_shape_map->end());

        (*it_kernel)->addKernel(p_klt_driver.build<Kernel<Annotation, Language, Runtime> >(kernel_desc));
      }

      for (it_shape_vect = shape_map.begin(); it_shape_vect != shape_map.end(); it_shape_vect++)
        for (it_shape = it_shape_vect->second.begin(); it_shape != it_shape_vect->second.end(); it_shape++)
          delete *it_shape;
      
    }
}



/** @} */

}

#endif /* __KLT_GENERATOR_HPP__ */

