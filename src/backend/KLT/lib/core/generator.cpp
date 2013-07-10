
#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"

#include "KLT/Core/data.hpp"

#include "KLT/Core/loop-selector.hpp"
#include "KLT/Core/data-flow.hpp"

#include "KLT/Core/cg-config.hpp"

#include "sage3basic.h"

#include <iostream>

namespace KLT {

namespace Core {

void Generator::buildArgumentLists(
  const std::map<Kernel *, LoopTrees::node_t *> & kernels_map,
  const std::set<SgVariableSymbol *> & parameters,
  const std::set<SgVariableSymbol *> & coefficients,
  const std::set<Data *> & datas_in,
  const std::set<Data *> & datas_out,
  const std::set<Data *> & datas_local
) {
  std::set<Data *> datas;
    datas.insert(datas_in.begin(),    datas_in.end());
    datas.insert(datas_out.begin(),   datas_out.end());
    datas.insert(datas_local.begin(), datas_local.end());

  std::map<Kernel *, LoopTrees::node_t *>::const_iterator it_kernel;
  for (it_kernel = kernels_map.begin(); it_kernel != kernels_map.end(); it_kernel++) {
    Kernel * kernel = it_kernel->first;
    LoopTrees::node_t * node = it_kernel->second;

    std::set<SgVariableSymbol *> sym_var_refs;
    collectReferencedSymbols(node, sym_var_refs);
    collectReferencedSymbols(datas, sym_var_refs);

    std::list<SgVariableSymbol *> parameters_argument_order;
    std::set<SgVariableSymbol *>::const_iterator it_parameter;
    for (it_parameter = parameters.begin(); it_parameter != parameters.end(); it_parameter++) {
      SgVariableSymbol * parameter = *it_parameter;

      bool used = sym_var_refs.find(parameter) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

      if (used) parameters_argument_order.push_back(parameter);
    }

    std::list<SgVariableSymbol *> coefficients_argument_order;
    std::set<SgVariableSymbol *>::const_iterator it_coefficient;
    for (it_coefficient = coefficients.begin(); it_coefficient != coefficients.end(); it_coefficient++) {
      SgVariableSymbol * coefficient = *it_coefficient;

      bool used = sym_var_refs.find(coefficient) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

      if (used) coefficients_argument_order.push_back(coefficient);
    }

    std::list<Data *> datas_argument_order;
    std::set<Data *>::const_iterator it_data;
    for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
      Data * data = *it_data;
      SgVariableSymbol * data_sym = data->getVariableSymbol(); // FIXME Whole data approx (same symbol same data)

      bool used = sym_var_refs.find(data_sym) != sym_var_refs.end();

      if (used) datas_argument_order.push_back(data); // If it is referenced (SgVarRefExp) then it is needed
    }
    // FIXME with partial data we might end up referencing multiple time the same variable..

    kernel->setArgument(datas_argument_order, parameters_argument_order, coefficients_argument_order);
  }
}

void Generator::generate(
  const LoopTrees & loop_trees,
  std::list<Kernel *> & kernels,
  const DataFlow & data_flow,
  const LoopSelector & loop_selector
) {
  assert(kernels.empty());

  std::map<Kernel *, LoopTrees::node_t *> kernels_map;

  // Create kernels for each loop-tree in the LoopTrees object. the list preserve the textual order of the trees and the map memorize which loops for which kernel 
  loop_selector.createKernels(loop_trees, kernels, kernels_map, this);

  // Data flow analysis: propagate flow-in and flow-out datas to the individual kernels
  data_flow.apply(loop_trees.getDatasIn(), loop_trees.getDatasOut(), loop_trees.getDatasLocal(), kernels, kernels_map);

  // Compute Argument List
  buildArgumentLists(kernels_map, loop_trees.getParameters(), loop_trees.getCoefficients(), loop_trees.getDatasIn(), loop_trees.getDatasOut(), loop_trees.getDatasLocal());

  // Fill the kernel with edited version of the loop trees
  loop_selector.extractNestedLoops(kernels_map);
}

Generator::Generator(SgProject * project) :
  p_driver(project),
  p_klt_driver(p_driver),
  p_sage_driver(p_driver)
{
  
}

Generator::~Generator() {}

void Generator::generate(const LoopTrees & loop_trees, std::list<Kernel *> & kernels, const CG_Config & cg_config) {
  assert(kernels.empty());

  generate(loop_trees, kernels, cg_config.getDataFlow(), cg_config.getLoopSelector());

  std::list<Kernel *>::iterator it_kernel;
  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    doCodeGeneration(*it_kernel, cg_config);
  }
}

}

}

