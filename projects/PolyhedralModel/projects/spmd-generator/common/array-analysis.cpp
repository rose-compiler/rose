
#include "common/array-analysis.hpp"

#include "rose.h"

ArrayPartition::ArrayPartition(RoseVariable & array_) :
  original_array(array_),
  dimensions(),
  type(NULL)
{
  type = original_array.getInitializedName()->get_type();
  while (isSgArrayType(type)) {
    SgArrayType * array_type = isSgArrayType(type);
    SgExpression * dim = array_type->get_index();
    assert(dim != NULL);

    SgUnsignedLongVal * dim_size = isSgUnsignedLongVal(dim);
    assert(dim_size != NULL);

    dimensions.insert(dimensions.begin(), dim_size->get_value());

    type = array_type->get_base_type();
  }
}

ArrayPartition::ArrayPartition(RoseVariable & array_, std::vector<unsigned> & dimensions_, SgType * type_) :
  original_array(array_),
  dimensions(dimensions_),
  type(type_)
{}

ArrayPartition::~ArrayPartition() {}

ArrayPartition * ArrayPartition::merge(ArrayPartition * p1, ArrayPartition * p2) {
  // TODO
}

ArrayAnalysis::ArrayAnalysis() :
  accesses_map()
{}

ArrayAnalysis::~ArrayAnalysis() {}

const std::pair<std::vector<ArrayPartition *>, std::vector<ArrayPartition *> > & ArrayAnalysis::get(SPMD_Tree * tree) const {
  std::map<SPMD_Tree *, std::pair<std::vector<ArrayPartition *>, std::vector<ArrayPartition *> > >::const_iterator it = accesses_map.find(tree);
  assert(it != accesses_map.end());
  return it->second;
}
    
void ArrayAnalysis::clear() {
  accesses_map.clear();
}

