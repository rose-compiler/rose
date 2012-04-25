
#include "common/array-analysis.hpp"

#include "rose.h"

ArrayAlias::ArrayAlias(ArrayPartition * original_array_) :
  original_array(original_array_)
{}

ArrayAlias::~ArrayAlias() {}

SgExpression * ArrayAlias::propagate(SgExpression * exp) const {
  SgPntrArrRefExp * arr_ref = isSgPntrArrRefExp(exp);
  SgVarRefExp * var_ref = isSgVarRefExp(exp);
  SgBinaryOp * bin_op = isSgBinaryOp(exp);
  SgUnaryOp * una_op = isSgUnaryOp(exp);
  SgFunctionCallExp * func_call = isSgFunctionCallExp(exp);

  if (arr_ref != NULL) return propagate(arr_ref);
  if (var_ref != NULL) return propagate(var_ref);

  if (bin_op != NULL) {
    bin_op->set_lhs_operand_i(propagate(bin_op->get_lhs_operand_i()));
    bin_op->set_rhs_operand_i(propagate(bin_op->get_rhs_operand_i()));
  }
  if (una_op != NULL) {
    una_op->set_operand_i(propagate(una_op->get_operand_i()));
  }
  if (func_call != NULL) {
    std::vector<SgExpression *> args = func_call->get_args()->get_expressions();
    for (int i = 0; i < args.size(); i++) {
      args[i] = propagate(args[i]);
      args[i]->set_parent(func_call->get_args());
    }
  }

  return exp;
}

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

const std::vector<unsigned> & ArrayPartition::getDimensions() const { return dimensions; }
SgType * ArrayPartition::getType() const { return type; }

ArrayPartition * ArrayPartition::merge(ArrayPartition * p1, ArrayPartition * p2) {
  // TODO
  assert(false);
  return NULL;
}

ArrayAnalysis::ArrayAnalysis() :
  accesses_map()
{}

ArrayAnalysis::~ArrayAnalysis() {}

const std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > & ArrayAnalysis::get(SPMD_Tree * tree) const {
  std::map<SPMD_Tree *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::const_iterator it = accesses_map.find(tree);
  assert(it != accesses_map.end());
  return it->second;
}

std::set<ArrayPartition *> * ArrayAnalysis::get_in(SPMD_Tree * tree) const {
  std::map<SPMD_Tree *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::const_iterator it = accesses_map.find(tree);
  assert(it != accesses_map.end());

  std::set<ArrayPartition *> * res = new std::set<ArrayPartition *>();

  std::set<ArrayPartition *>::const_iterator it1 = it->second.first.begin();
  std::set<ArrayPartition *>::const_iterator it2 = it->second.second.begin();

  while (it1 != it->second.first.end() && it2 != it->second.second.end()) {
    if (*it1 == *it2) { it1++; it2++; }
    else if (*it1 < *it2) { res->insert(*it1); it1++; }
    else it2++;
  }
  assert(it1 == it->second.first.end() || it2 == it->second.second.end());
  while (it1 != it->second.first.end()) {
    res->insert(*it1);
    it1++;
  }

  return res;
}

std::set<ArrayPartition *> * ArrayAnalysis::get_out(SPMD_Tree * tree) const {
  std::map<SPMD_Tree *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::const_iterator it = accesses_map.find(tree);
  assert(it != accesses_map.end());

  std::set<ArrayPartition *> * res = new std::set<ArrayPartition *>();

  std::set<ArrayPartition *>::const_iterator it1 = it->second.first.begin();
  std::set<ArrayPartition *>::const_iterator it2 = it->second.second.begin();

  while (it1 != it->second.first.end() && it2 != it->second.second.end()) {
    if (*it1 == *it2) { it1++; it2++; } 
    else if (*it2 < *it1) { res->insert(*it2); it2++; }
    else it1++;
  }
  assert(it1 == it->second.first.end() || it2 == it->second.second.end());
  while (it2 != it->second.second.end()) {
    res->insert(*it2);
    it2++;
  }

  return res;
}

std::set<ArrayPartition *> * ArrayAnalysis::get_inout(SPMD_Tree * tree) const {
  std::map<SPMD_Tree *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::const_iterator it = accesses_map.find(tree);
  assert(it != accesses_map.end());

  std::set<ArrayPartition *> * res = new std::set<ArrayPartition *>();

  std::set<ArrayPartition *>::const_iterator it1 = it->second.first.begin();
  std::set<ArrayPartition *>::const_iterator it2 = it->second.second.begin();
  
  while (it1 != it->second.first.end() && it2 != it->second.second.end()) {
    if (*it1 == *it2) { res->insert(*it1); it1++; it2++; }
    else if (*it1 < *it2) { it1++; }
    else it2++;
  }
  assert(it1 == it->second.first.end() || it2 == it->second.second.end());

  return res;
}
   
void ArrayAnalysis::clear() {
  accesses_map.clear();
}

