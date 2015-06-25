
#ifndef __KLT_GENERATOR_HPP__
#define __KLT_GENERATOR_HPP__

#include <map>
#include <set>
#include <list>
#include <string>
#include <iostream>

#include "KLT/Core/mfb-klt.hpp"

#ifndef VERBOSE
#define VERBOSE 0
#endif

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
    unsigned long p_file_id;

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
  std::vector<SgVariableSymbol *>::const_iterator it_symbol;
  typename std::set<Data<Annotation> *>::const_iterator it_data;
  typename std::vector<Data<Annotation> *>::const_iterator it_data_vect;

  const std::vector<SgVariableSymbol *> & parameters = loop_trees.getParameters();
  const std::vector<SgVariableSymbol *> & scalars = loop_trees.getScalars();
  const std::vector<Data<Annotation> *> & privates = loop_trees.getPrivates();

  std::set<SgVariableSymbol *> sym_var_refs;
    collectReferencedSymbols(kernel, sym_var_refs);

  // Parameter

//std::cerr << ">> parameters.size() = " << parameters.size() << std::endl;
  for (it_symbol = parameters.begin(); it_symbol != parameters.end(); it_symbol++) {
    SgVariableSymbol * parameter = *it_symbol;

    bool used = sym_var_refs.find(parameter) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().parameters.push_back(parameter);
  }
//std::cerr << ">> kernel->getArguments().parameters.size() = " << kernel->getArguments().parameters.size() << std::endl << std::endl;

  // Scalar

//std::cerr << ">> scalars.size() = " << scalars.size() << std::endl;
  for (it_symbol = scalars.begin(); it_symbol != scalars.end(); it_symbol++) {
    SgVariableSymbol * scalar = *it_symbol;

    bool used = sym_var_refs.find(scalar) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().scalars.push_back(scalar);
  }
//std::cerr << ">> kernel->getArguments().scalars.size() = "    << kernel->getArguments().scalars.size() << std::endl << std::endl;

  // Private

//std::cerr << ">> privates.size() = " << privates.size() << std::endl;
  for (it_data_vect = privates.begin(); it_data_vect != privates.end(); it_data_vect++) {
    Data<Annotation> * data = *it_data_vect;
    SgVariableSymbol * private_sym = data->getVariableSymbol();

    bool used = sym_var_refs.find(private_sym) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().privates.push_back(data);
  }
//std::cerr << ">> kernel->getArguments().privates.size() = "   << kernel->getArguments().privates.size() << std::endl << std::endl;

  // Data

//std::cerr << ">> kernel->getDataflow().datas.size() = " << kernel->getDataflow().datas.size() << std::endl;
  std::vector<Data<Annotation> *> sorted_data;
  for (it_data = kernel->getDataflow().datas.begin(); it_data != kernel->getDataflow().datas.end(); it_data++) {
    Data<Annotation> * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol(); // FIXME Whole data approx (same symbol same data)

    bool used = sym_var_refs.find(data_sym) != sym_var_refs.end();

    if (used) sorted_data.push_back(data); // If it is referenced (SgVarRefExp) then it is needed
  }
  std::sort(sorted_data.begin(), sorted_data.end(), &cmpDataName<Annotation>);
  kernel->getArguments().datas.insert(kernel->getArguments().datas.end(), sorted_data.begin(), sorted_data.end());
//std::cerr << ">> kernel->getArguments().datas.size() = "      << kernel->getArguments().datas.size() << std::endl << std::endl;

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
{
  Runtime::useSymbolsKernel(p_sage_driver, p_file_id);
}

template <class Annotation, class Language, class Runtime, class Driver>
Generator<Annotation, Language, Runtime, Driver>::~Generator() {}

/// Helper function to build set of all tiling configuration.
template <class Annotation, class Language, class Runtime>
void buildAllTileConfigs(
  std::map<typename LoopTrees<Annotation>::loop_t *, typename LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *> curr_elem,
  typename std::map<typename LoopTrees<Annotation>::loop_t *,  std::vector<typename LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *> >::const_iterator curr_it,
  const typename std::map<typename LoopTrees<Annotation>::loop_t *,  std::vector<typename LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *> >::const_iterator end_it,
  std::set<std::map<typename LoopTrees<Annotation>::loop_t *, typename LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *> > & tiling_set
) {
  typedef typename LoopTrees<Annotation>::loop_t loop_t;
  typedef typename LoopTiler<Annotation, Language, Runtime>::loop_tiling_t loop_tiling_t;

  if (curr_it == end_it) {
    tiling_set.insert(curr_elem);
  }
  else {
    typename std::map<loop_t *,  std::vector<loop_tiling_t *> >::const_iterator new_curr_it = curr_it;
    new_curr_it++;

    if (curr_it->second.empty()) {
      curr_elem.insert(std::pair<loop_t *, loop_tiling_t *>(curr_it->first, NULL));
      buildAllTileConfigs<Annotation, Language, Runtime>(curr_elem, new_curr_it, end_it, tiling_set);
    }
    else {
      typename std::vector<loop_tiling_t *>::const_iterator it;
      for (it = curr_it->second.begin(); it != curr_it->second.end(); it++) {
        std::map<loop_t *, loop_tiling_t *> new_curr_elem(curr_elem);
        new_curr_elem.insert(std::pair<loop_t *, loop_tiling_t *>(curr_it->first, *it));
        buildAllTileConfigs<Annotation, Language, Runtime>(new_curr_elem, new_curr_it, end_it, tiling_set);
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
  typedef typename LoopTrees<Annotation>::loop_t loop_t;
  typedef typename LoopTiler<Annotation, Language, Runtime>::loop_tiling_t loop_tiling_t;

  typename std::set<std::list<Kernel<Annotation, Language, Runtime> *> >::const_iterator it_kernel_list;
  typename std::list<Kernel<Annotation, Language, Runtime> *>::const_iterator it_kernel;
  typename std::set<IterationMap<Annotation, Language, Runtime> *>::const_iterator it_iteration_map;

  typename DataFlow<Annotation, Language, Runtime>::context_t df_ctx;

  // 0 - init data flow

  cg_config.getDataFlow().createContextFromLoopTree(loop_trees, df_ctx);
  cg_config.getDataFlow().markSplittedData(df_ctx);

  // 1 - Loop Selection : Generate multiple list of kernel that implement the given LoopTree

  cg_config.getLoopMapper().createKernels(loop_trees, kernel_lists);

  // 2 - Data Flow : performs data-flow analysis for each list of kernel

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++)
    cg_config.getDataFlow().generateFlowSets(*it_kernel_list, df_ctx);

  // 3 - Arguments : determines the list of arguments needed by each kernel

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++)
    for (it_kernel = it_kernel_list->begin(); it_kernel != it_kernel_list->end(); it_kernel++)
      buildArgumentLists(loop_trees, *it_kernel);

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++)
    for (it_kernel = it_kernel_list->begin(); it_kernel != it_kernel_list->end(); it_kernel++) {

      // 4 - Iterations Mapping : determines the "shape" of every loop of each kernel.
      //     The "shape" of a loop is how this loop is adapted to the execution model.

      std::map<loop_t *, std::vector<loop_tiling_t *> > tiling_map;

      cg_config.getLoopTiler().determineTiles(*it_kernel, tiling_map);

      std::set<std::map<loop_t *, loop_tiling_t *> > loop_tiling_set;
      buildAllTileConfigs<Annotation, Language, Runtime>(
        std::map<loop_t *, loop_tiling_t *>(),
        tiling_map.begin(),
        tiling_map.end(),
        loop_tiling_set
      );

      // 5 - Code Generation

      unsigned cnt = 0;
      typename std::set<std::map<loop_t *, loop_tiling_t *> >::iterator it_loop_tiling_map;
      for (it_loop_tiling_map = loop_tiling_set.begin(); it_loop_tiling_map != loop_tiling_set.end(); it_loop_tiling_map++) {


        typename ::MFB::KLT<Kernel<Annotation, Language, Runtime> >::object_desc_t kernel_desc(cnt++, *it_kernel, p_file_id);

        kernel_desc.tiling.insert(it_loop_tiling_map->begin(), it_loop_tiling_map->end());

        typename Kernel<Annotation, Language, Runtime>::kernel_desc_t * kernel = p_klt_driver.build<Kernel<Annotation, Language, Runtime> >(kernel_desc);

        (*it_kernel)->addKernel(kernel);
      }
      typename std::map<loop_t *, std::vector<loop_tiling_t *> >::const_iterator it_tiling_vect;
      typename std::vector<loop_tiling_t *>::const_iterator it_tiling;
      for (it_tiling_vect = tiling_map.begin(); it_tiling_vect != tiling_map.end(); it_tiling_vect++)
        for (it_tiling = it_tiling_vect->second.begin(); it_tiling != it_tiling_vect->second.end(); it_tiling++)
          delete *it_tiling;
      
    }
}



/** @} */

}

#endif /* __KLT_GENERATOR_HPP__ */

