
#include "common/spmd-tree.hpp"
#include "common/comm-analysis.hpp"

#include "toolboxes/algebra-container.hpp"

#include "rose.h"

#include <utility>

SPMD_Tree::SPMD_Tree(SPMD_Tree * parent_) :
  parent(parent_),
  children()
{}

SPMD_Tree::~SPMD_Tree() {}

void SPMD_Tree::appendChild(SPMD_Tree * child) {
  children.push_back(child);
}

void SPMD_Tree::prependChild(SPMD_Tree * child) {
  children.insert(children.begin(), child);
}

void SPMD_Tree::deepDelete() {
  std::vector<SPMD_Tree *>::iterator it;
  for (it = children.begin(); it != children.end(); it++) (*it)->deepDelete();
  delete this;
}

std::vector<SPMD_Tree *> & SPMD_Tree::getChildren() {
  return children;
}

SPMD_Tree * SPMD_Tree::getParent() const { return parent; }

SPMD_Root::SPMD_Root(SPMD_Tree * parent_) :
  SPMD_Tree(parent_)
{}

SPMD_Root::~SPMD_Root() {}

void SPMD_Root::print(std::ostream & out) const {
  out << "SPMD_Root";
}

SPMD_NativeStmt::SPMD_NativeStmt(SPMD_Tree * parent_, SgStatement * stmt_) :
  SPMD_Tree(parent_),
  stmt(stmt_)
{}

SPMD_NativeStmt::~SPMD_NativeStmt() {}

SgStatement * SPMD_NativeStmt::getStatement() { return stmt; }

void SPMD_NativeStmt::print(std::ostream & out) const {
  out << "SPMD_NativeStmt";
}

SPMD_Loop::SPMD_Loop(SPMD_Tree * parent_, RoseVariable iterator_, Domain * domain_) :
  SPMD_Tree(parent_),
  iterator(iterator_),
  domain(domain_)
{}

SPMD_Loop::~SPMD_Loop() {
  delete domain;
}

RoseVariable & SPMD_Loop::getIterator() { return iterator; }

Domain * SPMD_Loop::getDomain() { return domain; }

void SPMD_Loop::print(std::ostream & out) const {
  out << "SPMD_Loop";
}

SPMD_DomainRestriction::SPMD_DomainRestriction(SPMD_Tree * parent_) :
  SPMD_Tree(parent_),
  restrictions()
{}

SPMD_DomainRestriction::~SPMD_DomainRestriction() {
  std::vector<std::pair<Expression *, bool> >::iterator it;
  for (it = restrictions.begin(); it != restrictions.end(); it++)
    delete it->first;
}

void SPMD_DomainRestriction::addRestriction(Expression * restriction, bool is_equality) {
  restrictions.push_back(std::pair<Expression *, bool>(restriction, is_equality));
}

std::vector<std::pair<Expression *, bool> > & SPMD_DomainRestriction::getRestriction() { return restrictions; }

void SPMD_DomainRestriction::print(std::ostream & out) const {
  out << "SPMD_DomainRestriction";
}

unsigned SPMD_KernelCall::id_cnt = 0;

SPMD_KernelCall::SPMD_KernelCall(SPMD_Tree * parent_, SPMD_Tree * first, SPMD_Tree * last) :
  SPMD_Tree(parent_),
  iterators(),
  restrictions(),
  ordered_iterators(),
  id(id_cnt++)
{
  bool contains_loop = false;
  if (first != last) {
    SPMD_Tree * current = last;
    while (current != first->parent) {
      assert(current != NULL);
      SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(current);
      SPMD_DomainRestriction * restriction = dynamic_cast<SPMD_DomainRestriction *>(current);
      if (loop != NULL) {
        assert(loop->domain != NULL);
        iterators.insert(std::pair<RoseVariable, Domain *>(loop->iterator, loop->domain->copy()));
        ordered_iterators.insert(ordered_iterators.begin(), loop->iterator);
        contains_loop = true;
      }
      else if (restriction != NULL) {
        std::vector<std::pair<Expression *, bool> >::iterator it_restrict;
        for (it_restrict = restriction->restrictions.begin(); it_restrict != restriction->restrictions.end(); it_restrict++) {
          assert(it_restrict->first != NULL);
          restrictions.push_back(std::pair<Expression *, bool>(it_restrict->first->copy(), it_restrict->second));
        }
      }
      current = current->parent;
    }
  }
  else {
    SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(first);
    assert(loop != NULL);
    assert(loop->domain != NULL);
    iterators.insert(std::pair<RoseVariable, Domain *>(loop->iterator, loop->domain->copy()));
    ordered_iterators.insert(ordered_iterators.begin(), loop->iterator);
    contains_loop = true;
  }
  assert(contains_loop);
}

SPMD_KernelCall::~SPMD_KernelCall() {
  std::map<RoseVariable, Domain *>::iterator it0;
  for (it0 = iterators.begin(); it0 != iterators.end(); it0++)
    delete it0->second;

  std::vector<std::pair<Expression *, bool> >::iterator it;
  for (it = restrictions.begin(); it != restrictions.end(); it++)
    delete it->first;
}

unsigned SPMD_KernelCall::getID() const { return id; }

std::vector<SgExpression *> * SPMD_KernelCall::generateDimensionSizes() const {
  std::vector<SgExpression *> * res = new std::vector<SgExpression *>();

  std::vector<RoseVariable>::const_iterator it0;
  for (it0 = ordered_iterators.begin(); it0 != ordered_iterators.end(); it0++) {
    std::map<RoseVariable, Domain *>::const_iterator it1 = iterators.find(*it0);
    res->push_back(it1->second->genNumberOfPoints());
  }

  return res;
}

const std::map<RoseVariable, Domain *> & SPMD_KernelCall::getIterators() const { return iterators; }

const std::vector<std::pair<Expression *, bool> >  & SPMD_KernelCall::getRestrictions() const { return restrictions; }

const std::vector<RoseVariable> & SPMD_KernelCall::getOrderedIterators() const { return ordered_iterators; }

void SPMD_KernelCall::print(std::ostream & out) const {
  out << "SPMD_KernelCall";
}

SPMD_Comm::SPMD_Comm(SPMD_Tree * parent_, CommDescriptor * comm_descriptor_, std::vector<Conditions *> & conditions_) :
  SPMD_Tree(parent_),
  comm_descriptor(comm_descriptor_),
  conditions(conditions_)
{}

SPMD_Comm::~SPMD_Comm() {
  delete comm_descriptor;
}

CommDescriptor * SPMD_Comm::getCommDescriptor() const { return comm_descriptor; }
const std::vector<Conditions *> & SPMD_Comm::getConditons() const { return conditions; }

void SPMD_Comm::print(std::ostream & out) const {
  out << "SPMD_Comm";
}

SPMD_Sync::SPMD_Sync(SPMD_Tree * parent_, SyncDescriptor * sync_descriptor_) :
  SPMD_Tree(parent_),
  sync_descriptor(sync_descriptor_)
{}

SPMD_Sync::~SPMD_Sync() {
  delete sync_descriptor;
}

void SPMD_Sync::print(std::ostream & out) const {
  out << "SPMD_Sync";
}

