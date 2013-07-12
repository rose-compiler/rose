
#include "KLT/OpenCL/work-size-shaper.hpp"

#include "sage3basic.h"

#include <cassert>

namespace KLT {

namespace OpenCL {

WorkSizeShape::WorkSizeShape(
  unsigned int number_dims,
  const std::map<SgVariableSymbol *, unsigned int> & iterator_to_seq_map,
  const std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> & iterator_to_loop_map
) :
  p_number_dims(number_dims),
  p_iterator_to_seq_map(iterator_to_seq_map),
  p_iterator_to_loop_map(iterator_to_loop_map),
  p_global_dims(),
  p_local_dims(),
  p_have_local_dims(false)
{}

WorkSizeShape::~WorkSizeShape() {}

unsigned int WorkSizeShape::getNumberDimensions() const { return p_number_dims; }

const std::vector<SgExpression *> & WorkSizeShape::getGlobalDimensions() const { return p_global_dims; }

const std::vector<SgExpression *> & WorkSizeShape::getLocalDimensions() const { return p_local_dims; }

bool WorkSizeShape::haveLocalDimensions() const { return p_have_local_dims; }



void WorkSizeShaper::generateShapes(
  const std::list< Core::LoopTrees::loop_t *> & nested_loops,
  const std::list<SgVariableSymbol *> & params,
  std::set<WorkSizeShape *> & shapes
) const {
  std::list< Core::LoopTrees::loop_t *>::const_iterator it_loop;

  std::map<SgVariableSymbol *, unsigned int> iterator_to_seq_map;
  std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> iterator_to_loop_map;

  unsigned int cnt = 0;
  for (it_loop = nested_loops.begin(); it_loop != nested_loops.end(); it_loop++) {
    Core::LoopTrees::loop_t * loop = *it_loop;
    iterator_to_seq_map.insert (            std::pair<SgVariableSymbol *, unsigned int> (loop->iterator, cnt++));
    iterator_to_loop_map.insert(std::pair<SgVariableSymbol *, Core::LoopTrees::loop_t *>(loop->iterator, loop));
  }

  generateShapes(nested_loops, params, iterator_to_seq_map, iterator_to_loop_map, shapes);
}

// **************************

Dummy_WorkSizeShape::Dummy_WorkSizeShape(
  const std::list< Core::LoopTrees::loop_t *> & nested_loops,
  const std::map<SgVariableSymbol *, unsigned int> & iterator_to_seq_map,
  const std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> & iterator_to_loop_map
) :
  WorkSizeShape(nested_loops.size(), iterator_to_seq_map, iterator_to_loop_map)
{
  p_global_dims.resize(nested_loops.size());
  std::list< Core::LoopTrees::loop_t *>::const_iterator it_loop;
  unsigned int cnt = 0;
  for (it_loop = nested_loops.begin(); it_loop != nested_loops.end(); it_loop++) {
    Core::LoopTrees::loop_t * loop = *it_loop;
    p_global_dims[cnt] = SageBuilder::buildSubtractOp(loop->upper_bound, loop->lower_bound);
    cnt++;
  }
}

Dummy_WorkSizeShape::~Dummy_WorkSizeShape() {}

std::pair<SgExpression *, SgExpression *> Dummy_WorkSizeShape::generateIteratorBounds(
  const std::vector<SgVariableSymbol *> & global_coordinates,
  const std::vector<SgVariableSymbol *> & local_coordinates,
  SgVariableSymbol * iterator
) {
  assert(p_have_local_dims == false);
  assert(local_coordinates.size() == 0);

  std::map<SgVariableSymbol *, unsigned int>::const_iterator it_iter_to_seq = p_iterator_to_seq_map.find(iterator);
  assert(it_iter_to_seq != p_iterator_to_seq_map.end()); // not associated with a loop
  assert(it_iter_to_seq->second < global_coordinates.size()); // Found ID does not match any global coordinates

  SgVariableSymbol * global_coord_sym = global_coordinates[it_iter_to_seq->second];

  std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *>::const_iterator it_iter_to_loop = p_iterator_to_loop_map.find(iterator);
  assert(it_iter_to_loop != p_iterator_to_loop_map.end());
  assert(it_iter_to_loop->second->lower_bound != NULL);

  SgExpression * iterator_value = SageBuilder::buildAddOp(
    SageBuilder::buildVarRefExp(global_coord_sym),
    SageInterface::copyExpression(it_iter_to_loop->second->lower_bound)
  );

  return std::pair<SgExpression *, SgExpression *>(iterator_value, NULL);
}

void Dummy_WorkSizeShaper::generateShapes(
  const std::list< Core::LoopTrees::loop_t *> & nested_loops,
  const std::list<SgVariableSymbol *> & params,
  const std::map<SgVariableSymbol *, unsigned int> & iterator_to_seq_map,
  const std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> & iterator_to_loop_map,
  std::set<WorkSizeShape *> & shapes
) const {
  shapes.insert(new Dummy_WorkSizeShape(nested_loops, iterator_to_seq_map, iterator_to_loop_map));
}

}

}
