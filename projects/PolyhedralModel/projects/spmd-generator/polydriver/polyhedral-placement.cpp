
#include "polydriver/polyhedral-placement.hpp"
#include "polydriver/polyhedral-driver.hpp"
#include "polydriver/polyhedral-utils.hpp"

#include "common/spmd-tree.hpp"

#include "toolboxes/algebra-container.hpp"

#include "rose.h"

#define DEBUG 0

PolyConditions::PolyConditions() :
  Conditions(),
  conditions(),
  is_empty(false)
{}

PolyConditions::PolyConditions(std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> & polyhedron) :
  Conditions(),
  conditions(),
  is_empty(false)
{
  regenFromPoly(polyhedron);
}

PolyConditions::PolyConditions(const PolyConditions & arg) :
  Conditions(arg),
  conditions(),
  is_empty(arg.is_empty)
{
  std::vector<std::pair<LinearExpression *, bool> >::const_iterator it;
  for (it = arg.conditions.begin(); it != arg.conditions.end(); it++)
    conditions.push_back(std::pair<LinearExpression *, bool>(it->first->copy(), it->second));
}

PolyConditions::~PolyConditions() {
  std::vector<std::pair<LinearExpression *, bool> >::iterator it;
  for (it = conditions.begin(); it != conditions.end(); it++)
    delete it->first;
}

SgExpression * PolyConditions::generate() const {
  std::vector<std::pair<LinearExpression *, bool> >::const_iterator it;
  SgExpression * tmp = conditions[0].first->generate();
  SgExpression * and_cond = conditions[0].second ? 
                   (SgBinaryOp *)SageBuilder::buildEqualityOp(tmp, SageBuilder::buildIntVal(0)) :
                   (SgBinaryOp *)SageBuilder::buildGreaterOrEqualOp(tmp, SageBuilder::buildIntVal(0));
  for (it = conditions.begin()+1; it != conditions.end(); it++) {
    tmp = it->first->generate();
    and_cond = SageBuilder::buildAndOp(
      and_cond,
      it->second ? 
             (SgBinaryOp *)SageBuilder::buildEqualityOp(tmp, SageBuilder::buildIntVal(0)) : 
             (SgBinaryOp *)SageBuilder::buildGreaterOrEqualOp(tmp, SageBuilder::buildIntVal(0))
    );
  }
  return and_cond;
}

bool PolyConditions::isTrue() const {
  return conditions.size() == 0;
}

void PolyConditions::print(std::ostream & out) const {
  if (is_empty)
    out << "FALSE";
  else if (conditions.size() == 0)
    out << "TRUE";
  else {
    std::vector<std::pair<LinearExpression *, bool> >::const_iterator it;
    for (it = conditions.begin(); it != conditions.end(); it++) {
      if (it != conditions.begin()) out << " && ";
      it->first->print(out);
      if (it->second)
        out << " == 0";
      else
        out << " >= 0";
    }
  }
}

void PolyConditions::remove(const RoseVariable & it) {
  std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> polyhedron = genPolyhedron();

  std::map<unsigned, RoseVariable>::iterator it_id_map;
  unsigned var_id = polyhedron.second->size();
  for (it_id_map = polyhedron.second->begin(); it_id_map != polyhedron.second->end(); it_id_map++) {
    assert(it_id_map->first < polyhedron.second->size()); // I just want to check that...
    if (it_id_map->second == it)
      var_id = it_id_map->first;
  }

  if (var_id == polyhedron.second->size()) {
    delete polyhedron.first;
    delete polyhedron.second;
    return;
  }

  conditions.clear();

  VariableID var(var_id);
  Parma_Polyhedra_Library::Variables_Set vars(var);
  polyhedron.first->remove_space_dimensions(vars);

  std::map<unsigned, RoseVariable> * id_map = new std::map<unsigned, RoseVariable>();
  for (it_id_map = polyhedron.second->begin(); it_id_map != polyhedron.second->end(); it_id_map++) {
    if (it_id_map->first < var_id) id_map->insert(std::pair<unsigned, RoseVariable>(it_id_map->first, it_id_map->second));
    else if (it_id_map->first > var_id) id_map->insert(std::pair<unsigned, RoseVariable>(it_id_map->first - 1, it_id_map->second));
  }

  delete polyhedron.second;
  polyhedron.second = id_map;

  regenFromPoly(polyhedron);
}

PolyConditions * PolyConditions::new_restricted_by(SPMD_DomainRestriction * dom_restrict) const {
  PolyConditions * res = this->copy();

  std::vector<std::pair<Expression *, bool> > & restricts = dom_restrict->getRestriction();
  std::vector<std::pair<Expression *, bool> >::const_iterator it;
  for (it = restricts.begin(); it != restricts.end(); it++) {
    LinearExpression * lin_exp = dynamic_cast<LinearExpression *>(it->first);
    assert(lin_exp != NULL);
    res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp->copy(), it->second));
  }

  return res->simplify();
}

PolyConditions * PolyConditions::new_extended_by(SPMD_KernelCall * kernel_call) const {
  PolyConditions * res = this->copy();

  const std::map<RoseVariable, Domain *> & its = kernel_call->getIterators();
  std::map<RoseVariable, Domain *>::const_iterator it_kc;
  for (it_kc = its.begin(); it_kc != its.end(); it_kc++) {
    LinearDomain * domain = dynamic_cast<LinearDomain *>(it_kc->second);
    assert(domain != NULL);
    const RoseVariable & it = it_kc->first;
    std::vector<std::pair<LinearExpression *, int> >::const_iterator it_b;
    const std::vector<std::pair<LinearExpression *, int> > & lb = domain->getRawLowerBound();
    for (it_b = lb.begin(); it_b != lb.end(); it_b++) {
      assert(it_b->second == 1); // FIXME
      LinearExpression * lin_exp = it_b->first->copy();
      assert(lin_exp->get(it) == 0);
      lin_exp->neg();
      lin_exp->set(it, 1);
      res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, false));
    }
    const std::vector<std::pair<LinearExpression *, int> > & ub = domain->getRawUpperBound();
    for (it_b = ub.begin(); it_b != ub.end(); it_b++) {
      assert(it_b->second == 1); // FIXME
      LinearExpression * lin_exp = it_b->first->copy();
      assert(lin_exp->get(it) == 0);
      lin_exp->set(it, -1);
      res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, false));
    }
  }

  return res->simplify();
}

PolyConditions * PolyConditions::new_restricted_by(SPMD_KernelCall * kernel_call) const {
  PolyConditions * res = this->copy();

  const std::vector<std::pair<Expression *, bool> > & restricts = kernel_call->getRestrictions();
  std::vector<std::pair<Expression *, bool> >::const_iterator it;
  for (it = restricts.begin(); it != restricts.end(); it++) {
    assert(it->first != NULL);
    LinearExpression * lin_exp = dynamic_cast<LinearExpression *>(it->first);
    assert(lin_exp != NULL);
    res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp->copy(), it->second));
  }

  return res->simplify();
}

PolyConditions * PolyConditions::new_without_first_it(SPMD_Loop * loop) const {
  PolyConditions * res = this->copy();

  LinearDomain * domain = dynamic_cast<LinearDomain *>(loop->getDomain());
  assert(domain != NULL);
  RoseVariable & it = loop->getIterator();
  assert(domain->getStride() == 1); // FIXME

  std::vector<std::pair<LinearExpression *, int> >::const_iterator it_b;
  const std::vector<std::pair<LinearExpression *, int> > & lb = domain->getRawLowerBound();
  for (it_b = lb.begin(); it_b != lb.end(); it_b++) {
    assert(it_b->second == 1); // FIXME
    LinearExpression * lin_exp = it_b->first->copy();
    assert(lin_exp->get(it) == 0);
    lin_exp->add(constantLBL(), 1);
    lin_exp->neg();
    lin_exp->set(it, 1);
    res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, false));
  }

  return res->simplify();
}

PolyConditions * PolyConditions::new_extended_by(SPMD_Loop * loop) const {
  PolyConditions * res = this->copy();

  LinearDomain * domain = dynamic_cast<LinearDomain *>(loop->getDomain());
  assert(domain != NULL);
  RoseVariable & it = loop->getIterator();
  assert(domain->getStride() == 1); // FIXME

  std::vector<std::pair<LinearExpression *, int> >::const_iterator it_b;
  const std::vector<std::pair<LinearExpression *, int> > & lb = domain->getRawLowerBound();
  for (it_b = lb.begin(); it_b != lb.end(); it_b++) {
    assert(it_b->second == 1); // FIXME
    LinearExpression * lin_exp = it_b->first->copy();
    assert(lin_exp->get(it) == 0);
    lin_exp->neg();
    lin_exp->set(it, 1);
    res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, false));
  }
  const std::vector<std::pair<LinearExpression *, int> > & ub = domain->getRawUpperBound();
  for (it_b = ub.begin(); it_b != ub.end(); it_b++) {
    assert(it_b->second == 1); // FIXME
    LinearExpression * lin_exp = it_b->first->copy();
    assert(lin_exp->get(it) == 0);
    lin_exp->set(it, -1);
    res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, false));
  }

  return res->simplify();
}

PolyConditions * PolyConditions::new_restricted_by(SPMD_Loop * loop, bool first_it) const {
  PolyConditions * res = this->copy();

  LinearDomain * domain = dynamic_cast<LinearDomain *>(loop->getDomain());
  assert(domain != NULL);
  RoseVariable & it = loop->getIterator();
  assert(domain->getStride() == 1); // FIXME

  if (first_it) {
    const std::vector<std::pair<LinearExpression *, int> > & lb = domain->getRawLowerBound();
    std::vector<std::pair<LinearExpression *, int> >::const_iterator it_lb;
    for (it_lb = lb.begin(); it_lb != lb.end(); it_lb++) {
      assert(it_lb->second == 1); // FIXME
      LinearExpression * lin_exp = it_lb->first->copy();
      assert(lin_exp->get(it) == 0);
      lin_exp->set(it, -1);
      res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, true));
    }
  }
  else {
    std::vector<std::pair<LinearExpression *, int> >::const_iterator it_ub;
    const std::vector<std::pair<LinearExpression *, int> > & ub = domain->getRawUpperBound();
    for (it_ub = ub.begin(); it_ub != ub.end(); it_ub++) {
      assert(it_ub->second == 1); // FIXME
      LinearExpression * lin_exp = it_ub->first->copy();
      assert(lin_exp->get(it) == 0);
      lin_exp->set(it, -1);
      res->conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, true));
    }
  }
  
  return res->simplify();
}

PolyConditions * PolyConditions::new_restricted_by(Conditions * cond) const {
  PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(cond);
  assert(poly_cond != NULL);

  PolyConditions * res = this->copy();

  std::vector<std::pair<LinearExpression *, bool> >::iterator it;
  for (it = poly_cond->conditions.begin(); it != poly_cond->conditions.end(); it++)
    res->conditions.push_back(std::pair<LinearExpression *, bool>(it->first->copy(), it->second));
  
  return res->simplify();
}

bool PolyConditions::equal(Conditions * cond) const {
  PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(cond);
  assert(poly_cond != NULL);

  assert(false); // FIXME don't think it will be used

  return true;
}

std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> PolyConditions::genPolyhedron() const {
  unsigned var_cnt = 0;
  std::map<RoseVariable, unsigned> var_id_map;
  { // Collect the variables
    std::map<RoseVariable, unsigned>::iterator it_var_id_map;
    std::vector<std::pair<LinearExpression *, bool> >::const_iterator it_lin_exp;
    std::map<RoseVariable, int>::const_iterator it_raw_lin_exp;
    for (it_lin_exp = conditions.begin(); it_lin_exp != conditions.end(); it_lin_exp++) {
      const std::map<RoseVariable, int> & raw_lin_exp = it_lin_exp->first->getRawData();
      for (it_raw_lin_exp = raw_lin_exp.begin(); it_raw_lin_exp != raw_lin_exp.end(); it_raw_lin_exp++) {
        if (isConstant(it_raw_lin_exp->first)) continue;
        it_var_id_map = var_id_map.find(it_raw_lin_exp->first);
        if (it_var_id_map == var_id_map.end())
          var_id_map.insert(std::pair<RoseVariable, unsigned>(it_raw_lin_exp->first, var_cnt++));
      }
    }
  }

  assert(var_id_map.size() == var_cnt);

  Polyhedron * polyhedron = new Polyhedron(var_cnt);

  { // Build the corresponding polyhedron
    std::vector<std::pair<LinearExpression *, bool> >::const_iterator it_lin_exp;
    std::map<RoseVariable, int>::const_iterator it_raw_lin_exp;
    for (it_lin_exp = conditions.begin(); it_lin_exp != conditions.end(); it_lin_exp++) {
      const std::map<RoseVariable, int> & raw_lin_exp = it_lin_exp->first->getRawData();
      LinearExpression_ppl le;
      for (it_raw_lin_exp = raw_lin_exp.begin(); it_raw_lin_exp != raw_lin_exp.end(); it_raw_lin_exp++) {
        if (isConstant(it_raw_lin_exp->first))
          le += it_raw_lin_exp->second;
        else
          le += it_raw_lin_exp->second * VariableID(var_id_map[it_raw_lin_exp->first]);
      }
      if (it_lin_exp->second)
        polyhedron->refine_with_constraint(le == 0);
      else
        polyhedron->refine_with_constraint(le >= 0);
    }
  }

  std::map<unsigned, RoseVariable> * invert_id_map = new std::map<unsigned, RoseVariable>();
  {
    std::map<RoseVariable, unsigned>::iterator it_var_id_map;
    for (it_var_id_map = var_id_map.begin(); it_var_id_map != var_id_map.end(); it_var_id_map++)
      invert_id_map->insert(std::pair<unsigned, RoseVariable>(it_var_id_map->second, it_var_id_map->first));
  }

  assert(var_cnt == invert_id_map->size());

  return std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *>(polyhedron, invert_id_map);
}

PolyConditions * PolyConditions::simplify() {
  if (conditions.size() == 0) return this;

  std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> polyhedron = genPolyhedron();

  regenFromPoly(polyhedron);

  return this;
}

void PolyConditions::regenFromPoly(std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> & polyhedron) {
  { // Clear the current conditions
    std::vector<std::pair<LinearExpression *, bool> >::iterator it_lin_exp;
    for (it_lin_exp = conditions.begin(); it_lin_exp != conditions.end(); it_lin_exp++)
      delete it_lin_exp->first;
    conditions.clear();
  }

  if (polyhedron.first->is_universe()) {
    assert(conditions.size() == 0);
    is_empty = false;
  }
  else if (!polyhedron.first->is_empty())  { // Reconstruct the Conditions if non_empty
    const ConstraintSystem & cs = polyhedron.first->minimized_constraints();
    ConstraintSystem::const_iterator it;
    for (it = cs.begin(); it != cs.end(); it++) {
      LinearExpression * lin_exp = new LinearExpression();
      for (int i = 0; i < polyhedron.second->size(); i++)
        if (it->coefficient(VariableID(i)).get_si() != 0)
          lin_exp->set((*(polyhedron.second))[i], it->coefficient(VariableID(i)).get_si());
      lin_exp->set(constantLBL(), it->inhomogeneous_term().get_si());
      if (it->is_equality())
        conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, true));
      else {
        if (it->is_strict_inequality())
          lin_exp->add(constantLBL(), -1);
        conditions.push_back(std::pair<LinearExpression *, bool>(lin_exp, false));
      }
    }
    is_empty = false;
  }
  else {
    is_empty = true;
  }

  std::vector<std::pair<LinearExpression *, bool> >::iterator it_lin_exp = conditions.begin();
  while (it_lin_exp != conditions.end())
    if (it_lin_exp->first->getRawData().size() == 0)
      it_lin_exp = conditions.erase(it_lin_exp);
    else
      it_lin_exp++;

  delete polyhedron.first;
  delete polyhedron.second;
}

PolyConditions * PolyConditions::copy() const {
  return new PolyConditions(*this);
}

PolyPlacement::PolyPlacement(ComputeSystem * cs) :
  NodePlacement(cs),
  polydriver(NULL)
{}

PolyPlacement::~PolyPlacement() {}

Conditions * PolyPlacement::genAssociatedFullCondition() const {
  return new PolyConditions();
}

struct PartialFunction {
  const std::map<RoseVariable, unsigned> & target_map;
  const std::map<unsigned, RoseVariable> & from_map;

  PartialFunction(const std::map<RoseVariable, unsigned> & target_map_, const std::map<unsigned, RoseVariable> & from_map_) :
    target_map(target_map_),
    from_map(from_map_)
  {
#if 0
    std::map<RoseVariable, unsigned>::const_iterator it_target;
    std::map<unsigned, RoseVariable>::const_iterator it_from;
    std::cerr << "Partial function build from:" << std::endl;
    std::cerr << "\ttarget_map:" << std::endl;
    for (it_target = target_map.begin(); it_target != target_map.end(); it_target++)
      std::cerr << "\t\t" << it_target->first.getString() << " -> " << it_target->second << std::endl;
    std::cerr << "\tfrom_map:" << std::endl;
    for (it_from = from_map.begin(); it_from != from_map.end(); it_from++)
      std::cerr << "\t\t" << it_from->first << " -> " << it_from->second.getString() << std::endl;
#endif
  }

  bool equal() const {
    if (target_map.size() != from_map.size()) return false;
    std::map<RoseVariable, unsigned>::const_iterator it_target;
    std::map<unsigned, RoseVariable>::const_iterator it_from;

    for (it_from = from_map.begin(); it_from != from_map.end(); it_from++) {
      it_target = target_map.find(it_from->second);
      if (it_target == target_map.end()) return false;
      if (it_target->second != it_from->first) return false;
    }
    return true;
  }

  bool maps(Parma_Polyhedra_Library::dimension_type i, Parma_Polyhedra_Library::dimension_type & j) const {
//    std::cerr << "PartialFunction::maps(" << i << ") : ";
    std::map<unsigned, RoseVariable>::const_iterator it = from_map.find(i);
    if (it == from_map.end()) {
//      std::cerr << "not in from" << std::endl;
      return false;
    }
//    std::cerr << "var=" << it->second.getString() << " : ";
    std::map<RoseVariable, unsigned>::const_iterator it_ = target_map.find(it->second);
    if (it_ == target_map.end()) {
//      std::cerr << "not in target" << std::endl;
      return false;
    }
//    std::cerr << " j=" << it_->second << std::endl;
    j = it_->second;
    return true;
  }

  bool has_empty_codomain() const {
    return target_map.size() == 0;
  }

  unsigned max_in_codomain() const {
    return target_map.size();
  }
};

void reorder(std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> & polyhedron, const std::map<RoseVariable, unsigned> & common_map) {
  if (polyhedron.second->size() < common_map.size()) {
    polyhedron.first->add_space_dimensions_and_embed(common_map.size() - polyhedron.second->size());
  }
  PartialFunction pf(common_map, *(polyhedron.second));
  polyhedron.first->map_space_dimensions(pf);
  polyhedron.second->clear();
  std::map<RoseVariable, unsigned>::const_iterator it;
  for (it = common_map.begin(); it != common_map.end(); it++)
    polyhedron.second->insert(std::pair<unsigned, RoseVariable>(it->second, it->first));
}

bool compare(
  std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> & p1,
  std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> & p2
) {
  return p1.first->contains(*p2.first) && p2.first->contains(*p1.first);
}

bool PolyPlacement::areEqual(const std::vector<Conditions *> & set1, const std::vector<Conditions *> & set2) const {
  if (set1.size() != set2.size()) return false;

  if (set1.size() == 1) {
    PolyConditions * poly_cond_1 = dynamic_cast<PolyConditions *>(set1[0]);
    PolyConditions * poly_cond_2 = dynamic_cast<PolyConditions *>(set2[1]);
    if (poly_cond_1->conditions.size() == 0 && poly_cond_2->conditions.size() == 0) return true;
    else if  (poly_cond_1->conditions.size() == 0 || poly_cond_2->conditions.size() == 0) return false;
  }

  std::vector<Conditions *>::const_iterator it_set;
  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > polyhedrons1;
  {
    for (it_set = set1.begin(); it_set != set1.end(); it_set++) {
      PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it_set);
      assert(poly_cond != NULL);
      polyhedrons1.push_back(poly_cond->genPolyhedron());
    }
  }

  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > polyhedrons2;
  {
    for (it_set = set2.begin(); it_set != set2.end(); it_set++) {
      PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it_set);
      assert(poly_cond != NULL);
      polyhedrons2.push_back(poly_cond->genPolyhedron());
    }
  }

  {
    std::map<RoseVariable, unsigned> common_map;
    std::set<RoseVariable> var_set;
    std::map<unsigned, RoseVariable>::iterator it_var_id_map;
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly;
    for (it_poly = polyhedrons1.begin(); it_poly != polyhedrons1.end(); it_poly++)
      for (it_var_id_map = it_poly->second->begin(); it_var_id_map != it_poly->second->end(); it_var_id_map++)
        var_set.insert(it_var_id_map->second);
    for (it_poly = polyhedrons2.begin(); it_poly != polyhedrons2.end(); it_poly++)
      for (it_var_id_map = it_poly->second->begin(); it_var_id_map != it_poly->second->end(); it_var_id_map++)
        var_set.insert(it_var_id_map->second);
    std::set<RoseVariable>::iterator it_var_set;
    unsigned var_cnt = 0;
    for (it_var_set = var_set.begin(); it_var_set != var_set.end(); it_var_set++)
      common_map.insert(std::pair<RoseVariable, unsigned>(*it_var_set, var_cnt++));

    for (it_poly = polyhedrons1.begin(); it_poly != polyhedrons1.end(); it_poly++)
      reorder(*it_poly, common_map);
    for (it_poly = polyhedrons2.begin(); it_poly != polyhedrons2.end(); it_poly++)
      reorder(*it_poly, common_map);
  }

  bool is_equal = true;
  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly_1;
  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly_2;
  for (it_poly_1 = polyhedrons1.begin(); it_poly_1 != polyhedrons1.end(); it_poly_1++) {
    bool found_equal = false;
    it_poly_2 = polyhedrons2.begin();
    while (it_poly_2 != polyhedrons2.end()) {
      found_equal = compare(*it_poly_1, *it_poly_2);
      if (found_equal) {
        polyhedrons2.erase(it_poly_2);
        break;
      }
    }
    if (!found_equal) { is_equal = false; break; }
  }

  { // cleanning
    for (it_poly_1 = polyhedrons1.begin(); it_poly_1 != polyhedrons1.end(); it_poly_1++) {
      delete it_poly_1->first;
      delete it_poly_1->second;
    }
    for (it_poly_1 = polyhedrons2.begin(); it_poly_1 != polyhedrons2.end(); it_poly_1++) {
      delete it_poly_1->first;
      delete it_poly_1->second;
    }
  }

  return is_equal;
}

void PolyPlacement::simplify(std::vector<Conditions *> & set) const {
  { // remove empty polyhedron and check for universe polyhedron
    bool have_universe = false;
    std::vector<Conditions *>::iterator it = set.begin();
    while (it != set.end()) {
      PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it);
      assert(poly_cond != NULL);
      if (poly_cond->is_empty) {
        it = set.erase(it);
        //delete poly_cond;
      }
      else if (poly_cond->conditions.size() == 0) {
        have_universe = true;
        break;
      }
      else {
        it++;
      }
    }
    if (have_universe) {
      it = set.begin();
      while (it != set.end()) {
        //delete *it;
        it++;
      }
      set.clear();
      set.push_back(new PolyConditions());
      return;
    }
  }

  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > polyhedrons;
  {
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly;
    std::vector<Conditions *>::iterator it_set;
    for (it_set = set.begin(); it_set != set.end(); it_set++) {
      PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it_set);
      assert(poly_cond);
      polyhedrons.push_back(poly_cond->genPolyhedron());
    }

    std::map<RoseVariable, unsigned> common_map;
    {
      std::set<RoseVariable> var_set;
      std::map<unsigned, RoseVariable>::iterator it_var_id_map;
      for (it_poly = polyhedrons.begin(); it_poly != polyhedrons.end(); it_poly++)
        for (it_var_id_map = it_poly->second->begin(); it_var_id_map != it_poly->second->end(); it_var_id_map++)
          var_set.insert(it_var_id_map->second);
      std::set<RoseVariable>::iterator it_var_set;
      unsigned var_cnt = 0;
      for (it_var_set = var_set.begin(); it_var_set != var_set.end(); it_var_set++)
        common_map.insert(std::pair<RoseVariable, unsigned>(*it_var_set, var_cnt++));
    }

    for (it_poly = polyhedrons.begin(); it_poly != polyhedrons.end(); it_poly++)
      reorder(*it_poly, common_map);
  }

  { // Remove included polyhedron
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly_;
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly = polyhedrons.begin();
    while (it_poly != polyhedrons.end()) {
      if (it_poly->first == NULL) { it_poly++; continue; }
      it_poly_ = polyhedrons.begin();
      while (it_poly_ != polyhedrons.end()) {
        if (it_poly == it_poly_) { it_poly_++; continue; }
        if (it_poly_->first != NULL && it_poly->first->contains(*(it_poly_->first))) {
          delete it_poly_->first;
          delete it_poly_->second;
          it_poly_->first = NULL;
        }
        it_poly_++;
      }
      it_poly++;
    }
    it_poly = polyhedrons.begin();
    while (it_poly != polyhedrons.end()) {
      if (it_poly->first == NULL)
        it_poly = polyhedrons.erase(it_poly);
      else
        it_poly++;
    }
  }

  // TODO other simplications...

  { // Regenerate
    std::vector<Conditions *>::iterator it = set.begin();
    while (it != set.end()) {
      //delete *it;
      it = set.erase(it);
    }
    set.clear();
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly;
    for (it_poly = polyhedrons.begin(); it_poly != polyhedrons.end(); it_poly++) {
      if (!it_poly->first->is_empty())  { // Reconstruct the Conditions if non_empty
        PolyConditions * poly_cond = new PolyConditions(*it_poly);
        set.push_back(poly_cond);
      }
    }
  }
}

void PolyPlacement::removeIterator(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_KernelCall * kernel_call
) const {
  const std::map<RoseVariable, Domain *> & its = kernel_call->getIterators();
  std::map<RoseVariable, Domain *>::const_iterator it_its;

  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++)
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it_cond);
        assert(poly_cond != NULL);
        // FIXME function to removea set of iterators...
        for (it_its = its.begin(); it_its != its.end(); it_its++)
          poly_cond->remove(it_its->first);
      }
      simplify(it_cs_map->second);
    }
}

void PolyPlacement::removeIterator(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_Loop * loop
) const {
  const RoseVariable & it = loop->getIterator();

  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++)
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it_cond);
        assert(poly_cond != NULL);
        poly_cond->remove(it);
      }
      simplify(it_cs_map->second);
    }
}

std::vector<Polyhedron *> * removePolyFromPoly(
    std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> original,
    std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> to_be_remove
) {
  std::vector<Polyhedron *> * res = new std::vector<Polyhedron *>();

#if DEBUG
  std::cerr << "Start removePolyFromPoly()" << std::endl;
#endif

  Polyhedron * copy = new Polyhedron(*(original.first));
  assert(copy->space_dimension() >= to_be_remove.first->space_dimension()); // FIXME it is a test
  if (copy->space_dimension() < to_be_remove.first->space_dimension()) {
    copy->add_space_dimensions_and_embed(to_be_remove.first->space_dimension() - copy->space_dimension());
  }

  if (
       copy->space_dimension() == to_be_remove.first->space_dimension() &&
       copy->contains(*to_be_remove.first) &&
       to_be_remove.first->contains(*copy)
  ) {
    delete copy;
    return res;
  }

  // Generate a map from to_be_remove's IDs to original's IDs
  std::map<unsigned, unsigned> trans_map;
  std::map<unsigned, RoseVariable>::iterator it_map_orignal;
  std::map<RoseVariable, unsigned> original_revert_map;
  for (it_map_orignal = original.second->begin(); it_map_orignal != original.second->end(); it_map_orignal++)
    original_revert_map.insert(std::pair<RoseVariable, unsigned>(it_map_orignal->second, it_map_orignal->first));
  std::map<unsigned, RoseVariable>::iterator it_map_to_be_remove;
  std::map<RoseVariable, unsigned>::iterator it_original_revert_map;
  for (it_map_to_be_remove = to_be_remove.second->begin(); it_map_to_be_remove != to_be_remove.second->end(); it_map_to_be_remove++) {
    it_original_revert_map = original_revert_map.find(it_map_to_be_remove->second);
    assert(it_original_revert_map != original_revert_map.end());
    trans_map.insert(std::pair<unsigned, unsigned>(it_map_to_be_remove->first, it_original_revert_map->second));
  }

  std::vector<Constraint> reversed_constraint;
  {
    const ConstraintSystem & cs = to_be_remove.first->minimized_constraints();
    ConstraintSystem::const_iterator it;
    for (it = cs.begin(); it != cs.end(); it++) {
      LinearExpression_ppl le;
      for (unsigned i = 0 ; i < it->space_dimension(); i++)
        le += it->coefficient(VariableID(i)) * VariableID(trans_map[i]);
      le += it->inhomogeneous_term();
      if (it->is_strict_inequality()) { // *it -> le > 0
        reversed_constraint.push_back(le <= 0);
      }
      else if (it->is_nonstrict_inequality()) { // *it -> le >= 0
        reversed_constraint.push_back(le <= -1);
      }
      else if (it->is_equality()) { // *it -> le == 0
        reversed_constraint.push_back(le >= 1);
        reversed_constraint.push_back(le <= -1);
      }
      else assert(false);
    }
  }

#if DEBUG
  std::cerr << "\tOriginal:" << std::endl;
  std::cerr << "\t\t"; copy->print(); std::cerr << std::endl; std::cerr << std::endl;
#endif
  std::vector<Constraint>::iterator it_reversed_constraint;
  for (it_reversed_constraint = reversed_constraint.begin(); it_reversed_constraint != reversed_constraint.end(); it_reversed_constraint++) {
#if DEBUG
    std::cerr << "\tAfter constraint:" << std::endl;
    std::cerr << "\t\t"; it_reversed_constraint->print(); std::cerr << std::endl;
#endif
    Polyhedron * refined_poly = new Polyhedron(*copy);
    refined_poly->refine_with_constraint(*it_reversed_constraint);
    // FIXME remove extra dimensions
    res->push_back(refined_poly);
#if DEBUG
    std::cerr << "\tit gives:" << std::endl;
    std::cerr << "\t\t"; refined_poly->print(); std::cerr << std::endl; std::cerr << std::endl;
#endif
  }

#if DEBUG
  std::cerr << "End   removePolyFromPoly()" << std::endl;
#endif

  delete copy;

  return res;
}

void PolyPlacement::removeFromCondSet(std::vector<Conditions *> & cond_set, const Conditions * to_be_remove) const {
#if DEBUG
  std::cerr << "PolyPlacement::removeFromCondSet" << std::endl;
  std::vector<Conditions *>::iterator it_cond_set;
  std::cerr << "\tOriginal Conds:" << std::endl;
  for (it_cond_set = cond_set.begin(); it_cond_set != cond_set.end(); it_cond_set++) {
    std::cerr << "\t\t"; (*it_cond_set)->print(std::cerr); std::cerr << std::endl;
  }
  std::cerr << "\tCond to remove:" << std::endl;
  std::cerr << "\t\t"; to_be_remove->print(std::cerr); std::cerr << std::endl;
#endif

  const PolyConditions * poly_cond_to_be_remove = dynamic_cast<const PolyConditions *>(to_be_remove);
  assert(poly_cond_to_be_remove != NULL);
  if (poly_cond_to_be_remove->isTrue()) { // it is universe
    cond_set.clear();
    return;
  }
  std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> poly_to_be_remove = poly_cond_to_be_remove->genPolyhedron();
#if DEBUG
  {
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > poly_set;
    std::cerr << "\tCond to remove Id/Var map:" << std::endl;
    std::map<unsigned, RoseVariable>::iterator it_id_var_map;
    for (it_id_var_map = poly_to_be_remove.second->begin(); it_id_var_map != poly_to_be_remove.second->end(); it_id_var_map++)
      std::cerr << "\t\t" << it_id_var_map->first << " -> " << it_id_var_map->second.getString() << std::endl;
    std::vector<Conditions *>::iterator it_set;
    for (it_set = cond_set.begin(); it_set != cond_set.end(); it_set++) {
      PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it_set);
      assert(poly_cond);
      poly_set.push_back(poly_cond->genPolyhedron());
    }
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly;
    std::cerr << "\tOriginal Conds Id/Var map:" << std::endl; unsigned cnt_cond = 1;
    for (it_poly = poly_set.begin(); it_poly != poly_set.end(); it_poly++) {
      std::cerr << "\t\t" << cnt_cond++ << " - " << std::endl;
      for (it_id_var_map = it_poly->second->begin(); it_id_var_map != it_poly->second->end(); it_id_var_map++) 
        std::cerr << "\t\t\t" << it_id_var_map->first << " -> " << it_id_var_map->second.getString() << std::endl;
    }
  }
#endif

  // Generate the polyhedrons
  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > poly_set;
  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly;
  {
    std::vector<Conditions *>::iterator it_set;
    for (it_set = cond_set.begin(); it_set != cond_set.end(); it_set++) {
      PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it_set);
      assert(poly_cond);
      poly_set.push_back(poly_cond->genPolyhedron());
    }
  }

  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > resulting_set;
  {
    for (it_poly = poly_set.begin(); it_poly != poly_set.end(); it_poly++) {
      std::vector<Polyhedron *> * res = removePolyFromPoly(*it_poly, poly_to_be_remove);
      for (std::vector<Polyhedron *>::iterator it = res->begin(); it != res->end(); it++)
        resulting_set.push_back(std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *>(
            *it, new std::map<unsigned, RoseVariable>(*(it_poly->second))
        ));
      delete res;
    }
  }
/*
  std::map<unsigned, std::pair<std::vector<Polyhedron *>, std::map<unsigned, RoseVariable> *> > poly_set_map;
  std::map<unsigned, std::pair<std::vector<Polyhedron *>, std::map<unsigned, RoseVariable> *> >::iterator it_poly_set_map;
  {
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > poly_set;
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly;
    std::vector<Conditions *>::iterator it_set;
    for (it_set = cond_set.begin(); it_set != cond_set.end(); it_set++) {
      PolyConditions * poly_cond = dynamic_cast<PolyConditions *>(*it_set);
      assert(poly_cond);
      poly_set.push_back(poly_cond->genPolyhedron());
    }

    std::map<unsigned, std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > > tmp_map;
    std::map<unsigned, std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > >::iterator it_tmp_map;
    for (it_poly = poly_set.begin(); it_poly != poly_set.end(); it_poly++) {
      it_tmp_map = tmp_map.find(it_poly->second->size());
      if (it_tmp_map == tmp_map.end())
        it_tmp_map = tmp_map.insert(std::pair<unsigned, std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > >(
            it_poly->second->size(), std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >()
        )).first;
      it_tmp_map->second.push_back(*it_poly);
    }
    it_tmp_map = tmp_map.find(0);
    if (it_tmp_map != tmp_map.end()) tmp_map.erase(it_tmp_map); // remove empty polyhedron
    
    for (it_tmp_map = tmp_map.begin(); it_tmp_map != tmp_map.end(); it_tmp_map++) {
      it_poly_set_map = poly_set_map.insert(std::pair<unsigned, std::pair<std::vector<Polyhedron *>, std::map<unsigned, RoseVariable> *> >(
          it_tmp_map->first, std::pair<std::vector<Polyhedron *>, std::map<unsigned, RoseVariable> *>(
              std::vector<Polyhedron *>(),
              new std::map<unsigned, RoseVariable>()
          )
      )).first;
      std::set<RoseVariable> var_set;
      for (it_poly = it_tmp_map->second.begin(); it_poly != it_tmp_map->second.end(); it_poly++) {
        std::map<unsigned, RoseVariable>::iterator it_var_id_map;
        for (it_var_id_map = it_poly->second->begin(); it_var_id_map != it_poly->second->end(); it_var_id_map++) {
          var_set.insert(it_var_id_map->second);
        }
      }
      std::set<RoseVariable>::iterator it_var_set;
      std::map<RoseVariable, unsigned> common_map;
      unsigned var_cnt = 0;
      for (it_var_set = var_set.begin(); it_var_set != var_set.end(); it_var_set++) {
        common_map.insert(std::pair<RoseVariable, unsigned>(*it_var_set, var_cnt));
        it_poly_set_map->second.second->insert(std::pair<unsigned, RoseVariable>(var_cnt, *it_var_set));
        var_cnt++;
      }
      assert(common_map.size() == it_tmp_map->first);
      for (it_poly = it_tmp_map->second.begin(); it_poly != it_tmp_map->second.end(); it_poly++) {
        PartialFunction pf(common_map, *(it_poly->second));
        if (!pf.equal()) it_poly->first->map_space_dimensions(pf);
        it_poly_set_map->second.first.push_back(it_poly->first);
      }
    }
  }

  // std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> poly_to_be_remove
  // std::map<unsigned, std::pair<std::vector<Polyhedron *>, std::map<unsigned, RoseVariable> *> > poly_set_map
  std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> > resulting_set;
  for (it_poly_set_map = poly_set_map.begin(); it_poly_set_map != poly_set_map.end(); it_poly_set_map++) {
    Polyhedron * local_poly_to_be_remove = new Polyhedron(*(poly_to_be_remove.first));
/*
    {
      std::map<unsigned, RoseVariable>::iterator it_var_id_map;

      // Variable set for 'poly_to_be_remove'
      std::set<RoseVariable> var_set_1;
      for (it_var_id_map = poly_to_be_remove.second->begin(); it_var_id_map != poly_to_be_remove.second->end(); it_var_id_map++) 
        var_set_1.insert(it_var_id_map->second);

      assert(var_set_1.size() == poly_to_be_remove.second->size());

      // Variable set for target
      std::set<RoseVariable> var_set_2;
      for (it_var_id_map = it_poly_set_map->second.second->begin(); it_var_id_map != it_poly_set_map->second.second->end(); it_var_id_map++)
        var_set_2.insert(it_var_id_map->second);

      assert(var_set_2.size() == it_poly_set_map->second.second->size());

      // Revert map for 'poly_to_be_remove'
      std::map<RoseVariable, unsigned> revert_map_1;
      for (it_var_id_map = poly_to_be_remove.second->begin(); it_var_id_map != poly_to_be_remove.second->end(); it_var_id_map++)
        revert_map_1.insert(std::pair<RoseVariable, unsigned>(it_var_id_map->second, it_var_id_map->first));

      // Revert map for target
      std::map<RoseVariable, unsigned> revert_map_2;
      for (it_var_id_map = it_poly_set_map->second.second->begin(); it_var_id_map != it_poly_set_map->second.second->end(); it_var_id_map++)
        revert_map_2.insert(std::pair<RoseVariable, unsigned>(it_var_id_map->second, it_var_id_map->first));

      std::set<RoseVariable>::iterator it;
      std::map<RoseVariable, unsigned>::iterator rit;

      // Set of var to remove from 'poly_to_be_remove'
      Parma_Polyhedra_Library::Variables_Set vars_1;
      for (it = var_set_1.begin(); it != var_set_1.end(); it++) {
        rit = revert_map_2.find(*it);
        if (rit != revert_map_2.end())
          vars_1.insert(revert_map_1[*it]);
      }

      // Set of var to remove from target
      Parma_Polyhedra_Library::Variables_Set vars_2;
      for (it = var_set_2.begin(); it != var_set_2.end(); it++) {
        rit = revert_map_1.find(*it);
        if (rit != revert_map_1.end())
          vars_1.insert(revert_map_2[*it]);
      }

      local_poly_to_be_remove->remove_space_dimensions(vars_1);
      std::vector<Polyhedron *>::iterator vit;
      for (vit = it_poly_set_map->second.first.begin(); vit != it_poly_set_map->second.first.end(); vit++)
        (*vit)->remove_space_dimensions(vars_2);
    }
*//*
    std::vector<Polyhedron *>::iterator it;
    std::vector<Polyhedron *> res;
    for (it = it_poly_set_map->second.first.begin(); it != it_poly_set_map->second.first.end(); it++) {
      std::vector<Polyhedron *> * tmp_res = removePolyFromPoly(*it, local_poly_to_be_remove);
      res.insert(res.end(), tmp_res->begin(), tmp_res->end());
      delete *it;
    }

    for (it = res.begin(); it != res.end(); it++)
      resulting_set.push_back(std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *>(
          *it, new std::map<unsigned, RoseVariable>(*(it_poly_set_map->second.second))
      ));

    delete it_poly_set_map->second.second;
    delete local_poly_to_be_remove;
  }
*/
  // TODO simplify: don't want to call simplify after regenerate, it would recreate poly...

  { // Regenerate
    std::vector<Conditions *>::iterator it = cond_set.begin();
    while (it != cond_set.end()) {
      //delete *it;
      it = cond_set.erase(it);
    }
    cond_set.clear();
    std::vector<std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> >::iterator it_poly;
    for (it_poly = resulting_set.begin(); it_poly != resulting_set.end(); it_poly++) {
      if (!it_poly->first->is_empty())  { // Reconstruct the Conditions if non_empty
        PolyConditions * poly_cond = new PolyConditions(*it_poly);
        cond_set.push_back(poly_cond);
      }
    }
  }

  simplify(cond_set); // FIXME see previous TODO

#if DEBUG
  std::cerr << "\tResulting Conds:" << std::endl;
  for (it_cond_set = cond_set.begin(); it_cond_set != cond_set.end(); it_cond_set++) {
    std::cerr << "\t\t"; (*it_cond_set)->print(std::cerr); std::cerr << std::endl;
  }
#endif
}

void PolyPlacement::setPolyDriver(PolyDriver * polydriver_) {
  polydriver = polydriver_;
}

bool PolyPlacement::isParallel(SPMD_Loop * loop) {
  PolyhedricAnnotation::PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & polyhedral_program =
      PolyhedricAnnotation::getPolyhedralProgram<SgStatement, SgExprStatement, RoseVariable>(polydriver->dymmy_stmt);

  std::map<SPMD_Tree *, ScopTree *>::iterator it_trees_map = polydriver->trees_map.find(loop);
  assert(it_trees_map != polydriver->trees_map.end());
  ScopTree * scoptree = it_trees_map->second;
  assert(scoptree != NULL);
  assert(scoptree->isLoop());
  ScopLoop * scoploop = (ScopLoop *)scoptree;

  std::vector<ScopStatement *> child_stmt;
  {
    std::queue<ScopTree *> queue;
    queue.push(scoploop);
    while (!queue.empty()) {
      if (queue.front()->isStatement())
        child_stmt.push_back((ScopStatement *)queue.front());
      else {
        const std::vector<ScopTree *> & scop_children = queue.front()->getChilds();
        std::vector<ScopTree *>::const_iterator it_child;
        for (it_child = scop_children.begin(); it_child != scop_children.end(); it_child++)
          queue.push(*it_child);
      }
      queue.pop();
    }
  }

  std::map<unsigned, std::pair<unsigned, unsigned> > stmt_id_it;
  {
    std::vector<ScopStatement *>::iterator it_child_stmt;
    for (it_child_stmt = child_stmt.begin(); it_child_stmt != child_stmt.end(); it_child_stmt++) {
      SgExprStatement * expr_stmt = (*it_child_stmt)->getExpression();
      PolyhedricAnnotation::Domain<SgStatement, SgExprStatement, RoseVariable> & domain =
          PolyhedricAnnotation::getDomain<SgStatement, SgExprStatement, RoseVariable>(expr_stmt);
      stmt_id_it.insert(std::pair<unsigned, std::pair<unsigned, unsigned> >(
        polyhedral_program.getStatementID(expr_stmt),
        std::pair<unsigned, unsigned>(
          polyhedral_program.getIteratorID(expr_stmt, scoploop->getIterator()),
          domain.getNumberOfIterators()
        )
      ));
    }
  }

  std::vector<Dependency *>::iterator it;
  for (it = polydriver->dependencies.begin(); it != polydriver->dependencies.end(); it++) {
    std::map<unsigned, std::pair<unsigned, unsigned> >::iterator it1 = stmt_id_it.find((*it)->getFrom());
    std::map<unsigned, std::pair<unsigned, unsigned> >::iterator it2 = stmt_id_it.find((*it)->getTo());
    if (it1 == stmt_id_it.end() || it2 == stmt_id_it.end()) continue;
    unsigned it_to = it2->second.first;
    unsigned it_from = it1->second.first + it2->second.second;
    const ConstraintSystem & cs = (*it)->getPolyhedron().constraints();
    ConstraintSystem::const_iterator it_cs;
    // TODO the maths
    for (it_cs = cs.begin(); it_cs != cs.end(); it_cs++) {
      if (it_cs->is_equality()) { // it is an equality
        if (it_cs->coefficient(VariableID(it_to)) == 0 && it_cs->coefficient(VariableID(it_from)) == 0) // Not implied in the constraint
          continue;
        else if (it_cs->coefficient(VariableID(it_to)) == 0 || it_cs->coefficient(VariableID(it_from)) == 0) // Only one implied
          goto end_cs_loop;
        else {
          for (int i = 0; i < it1->second.second + it2->second.second; i++) {
            if ((i != it_to) && (i != it_from) && (it_cs->coefficient(VariableID(i)) != 0))
              goto end_cs_loop;
          }
          if (it_cs->inhomogeneous_term().get_si() != 0)
            goto end_cs_loop;
        }
      }
    }
    continue;
end_cs_loop:
    std::cerr << "Loop " << loop << " is not parallel" << std::endl;
    (*it)->print(std::cerr);
    return false;
  }

  return true;
}

