
#include "KLT/Core/generator.hpp"

#include "KLT/Core/loop-selector.hpp"
#include "KLT/Core/data-flow.hpp"

#include "KLT/Core/cg-config.hpp"

#include "sage3basic.h"

#include <iostream>

namespace KLT {

namespace Core {

void Generator::generate(const LoopTrees & loop_trees, std::list<Kernel *> & kernels, const DataFlow & data_flow, const LoopSelector & loop_selector) {
  assert(kernels.empty());

  std::map<Kernel *, LoopTrees::node_t *> kernels_map;

  // Create kernels for each loop-tree in the LoopTrees object. the list preserve the textual order of the trees and the map memorize which loops for which kernel 
  loop_selector.createKernels(loop_trees, kernels, kernels_map, this);

  // Data flow analysis: propagate flow-in and flow-out datas to the individual kernels
  data_flow.apply(loop_trees.getDatasIn(), loop_trees.getDatasOut(), loop_trees.getDatasLocal(), kernels, kernels_map);

  // Fill the kernel with edited version of the loop trees
  loop_selector.extractNestedLoops(kernels_map);
}

Generator::Generator(SgProject * project) :
  p_project(project)
{}

Generator::~Generator() {}

void Generator::generate(const LoopTrees & loop_trees, std::list<Kernel *> & kernels, const CG_Config & cg_config) {
  assert(kernels.empty());

  generate(loop_trees, kernels, cg_config.getDataFlow(), cg_config.getLoopSelector());

  std::list<Kernel *>::iterator it_kernel;
  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++)
    doCodeGeneration(*it_kernel, cg_config);

}


}

}

