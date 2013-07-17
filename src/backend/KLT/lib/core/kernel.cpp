
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-trees.hpp"

#include <cassert>

namespace KLT {

namespace Core {

unsigned long Kernel::id_cnt = 0;

Kernel::Kernel(LoopTrees::node_t * root) :
  id(id_cnt++),
  p_root(root),
  p_data_flow(),
  p_argument_order(),
  p_loop_mappings()
{}

Kernel::~Kernel() {}

void Kernel::setRoot(LoopTrees::node_t * root) { p_root = root; }

LoopTrees::node_t * Kernel::getRoot() const { return p_root; }

Kernel::dataflow_t & Kernel::getDataflow() { return p_data_flow; }

const Kernel::dataflow_t & Kernel::getDataflow() const { return p_data_flow; }

Kernel::arguments_t & Kernel::getArguments() { return p_argument_order; }

const Kernel::arguments_t & Kernel::getArguments() const { return p_argument_order; }

std::set<Kernel::loop_mapping_t *> & Kernel::getLoopMappings() { return p_loop_mappings; }

const std::set<Kernel::loop_mapping_t *> & Kernel::getLoopMappings() const { return p_loop_mappings; }

void collectReferencedSymbols(Kernel * kernel, std::set<SgVariableSymbol *> & symbols) {
  assert(kernel->getRoot() != NULL); // Root should have been setup
  collectReferencedSymbols(kernel->getRoot(), symbols);

  assert(!kernel->getDataflow().datas.empty()); // Should always have some data
  collectReferencedSymbols(kernel->getDataflow().datas, symbols);
}

}

}

