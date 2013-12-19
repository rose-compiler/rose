
#ifdef __NO_TEMPLATE_DEFINITION__
#error "Compiling a template definition file while the macro __NO_TEMPLATE_DEFINITION__ is defined."
#endif

#include "KLT/Core/kernel.hpp"
#include "KLT/Core/data.hpp"

#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/loop-mapper.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/iteration-mapper.hpp"

#include "KLT/Core/mfb-klt.hpp"

#include "sage3basic.h"

#include <cassert>

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

template <class Kernel, class Driver>
typename Kernel::a_kernel * Generator<Kernel, Driver>::callToKernelBuilder(
  Kernel * kernel,
  typename Kernel::loop_mapping_t * loop_mapping,
  IterationMap<Kernel> * iteration_map
) const {
  typename ::MultiFileBuilder::KLT<Kernel>::object_desc_t kernel_desc(kernel, loop_mapping, iteration_map, p_file_id);
  typename ::MultiFileBuilder::KLT<Kernel>::build_result_t result =  p_klt_driver.build<Kernel>(kernel_desc);
  return result;
}

template <class Kernel, class Driver>
void Generator<Kernel, Driver>::buildArgumentLists(const LoopTrees & loop_trees, Kernel * kernel) {
  std::set<SgVariableSymbol *>::const_iterator it_symbol;
  std::set<Data *>::const_iterator it_data;

  const std::set<SgVariableSymbol *> & parameters = loop_trees.getParameters();
  const std::set<SgVariableSymbol *> & coefficients = loop_trees.getCoefficients();

  std::set<SgVariableSymbol *> sym_var_refs;
    collectReferencedSymbols(kernel, sym_var_refs);

  for (it_symbol = parameters.begin(); it_symbol != parameters.end(); it_symbol++) {
    SgVariableSymbol * parameter = *it_symbol;

    bool used = sym_var_refs.find(parameter) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().parameters.push_back(parameter);
  }

  for (it_symbol = coefficients.begin(); it_symbol != coefficients.end(); it_symbol++) {
    SgVariableSymbol * coefficient = *it_symbol;

    bool used = sym_var_refs.find(coefficient) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().coefficients.push_back(coefficient);
  }

  for (it_data = kernel->getDataflow().datas.begin(); it_data != kernel->getDataflow().datas.end(); it_data++) {
    Data * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol(); // FIXME Whole data approx (same symbol same data)

    bool used = sym_var_refs.find(data_sym) != sym_var_refs.end();

    if (used) kernel->getArguments().datas.push_back(data); // If it is referenced (SgVarRefExp) then it is needed
  }

  // FIXME with partial data we might end up referencing multiple time the same variable..
}

template <class Kernel, class Driver>
Generator<Kernel, Driver>::Generator(Driver & driver, const std::string & file_name) :
  p_driver(driver),
  p_klt_driver(p_driver),
  p_sage_driver(p_driver),
  p_file_name(file_name),
  p_file_id(createFile())
{}

template <class Kernel, class Driver>
Generator<Kernel, Driver>::~Generator() {}

template <class Kernel, class Driver>
void Generator<Kernel, Driver>::generate(const LoopTrees & loop_trees, std::set<std::list<Kernel *> > & kernel_lists, const CG_Config<Kernel> & cg_config) {
  typename std::set<std::list<Kernel *> >::const_iterator it_kernel_list;
  typename std::list<Kernel *>::const_iterator it_kernel;
  typename std::set<typename Kernel::loop_mapping_t  *>::const_iterator it_loop_mapping;
  typename std::set<Core::IterationMap<Kernel> *>::const_iterator it_iteration_map;

  // 1 - Loop Selection

  cg_config.getLoopMapper().createKernels(loop_trees, kernel_lists);

  // 2 - Data Flow

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    const std::list<Kernel *> & kernel_list = *it_kernel_list;
    cg_config.getDataFlow().generateFlowSets(loop_trees, kernel_list);
  }

  // 3 - Arguments
  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    const std::list<Kernel *> & kernel_list = *it_kernel_list;
    for (it_kernel = kernel_list.begin(); it_kernel != kernel_list.end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      buildArgumentLists(loop_trees, kernel);
    }
  }

  // 4 - Iterations Mapping

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    const std::list<Kernel *> & kernel_list = *it_kernel_list;
    for (it_kernel = kernel_list.begin(); it_kernel != kernel_list.end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      const std::set<typename Kernel::loop_mapping_t *> & loop_mappings = kernel->getLoopMappings();
      for (it_loop_mapping = loop_mappings.begin(); it_loop_mapping != loop_mappings.end(); it_loop_mapping++) {
        typename Kernel::loop_mapping_t  * loop_mapping = *it_loop_mapping;
        cg_config.getIterationMapper().generateShapes(loop_mapping, kernel->getIterationMaps(loop_mapping));
      }
    }
  }

  // 5 - Code Generation

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    const std::list<Kernel *> & kernel_list = *it_kernel_list;
    for (it_kernel = kernel_list.begin(); it_kernel != kernel_list.end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      const std::set<typename Kernel::loop_mapping_t  *> & loop_mappings = kernel->getLoopMappings();
      for (it_loop_mapping = loop_mappings.begin(); it_loop_mapping != loop_mappings.end(); it_loop_mapping++) {
        typename Kernel::loop_mapping_t * loop_mapping = *it_loop_mapping;
        const std::set<Core::IterationMap<Kernel> *> & iteration_maps =  kernel->getIterationMaps(loop_mapping);
        for (it_iteration_map = iteration_maps.begin(); it_iteration_map != iteration_maps.end(); it_iteration_map++) {
          Core::IterationMap<Kernel> * iteration_map = *it_iteration_map;
          kernel->setKernel(loop_mapping, iteration_map, callToKernelBuilder(kernel, loop_mapping, iteration_map));
        }
      }
    }
  }
}

/** @} */

}

}

