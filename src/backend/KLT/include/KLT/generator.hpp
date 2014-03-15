
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

  for (it_data = kernel->getDataflow().datas.begin(); it_data != kernel->getDataflow().datas.end(); it_data++) {
    Data<Annotation> * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol(); // FIXME Whole data approx (same symbol same data)

    bool used = sym_var_refs.find(data_sym) != sym_var_refs.end();

    if (used) kernel->getArguments().datas.push_back(data); // If it is referenced (SgVarRefExp) then it is needed
  }

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
      typename std::set<std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> >::iterator it_loop_shape;
      for (it_loop_shape = loop_shape_set.begin(); it_loop_shape != loop_shape_set.end(); it_loop_shape++) {
        typename ::MFB::KLT<Kernel<Annotation, Language, Runtime> >::object_desc_t kernel_desc(cnt++, *it_kernel, p_file_id);

        kernel_desc.shapes.insert(it_loop_shape->begin(), it_loop_shape->end());

        (*it_kernel)->addKernel(p_klt_driver.build<Kernel<Annotation, Language, Runtime> >(kernel_desc));
      }
      
    }
}



/** @} */

}

#endif /* __KLT_GENERATOR_HPP__ */

