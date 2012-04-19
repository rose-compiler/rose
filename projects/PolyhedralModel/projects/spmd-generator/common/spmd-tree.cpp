
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

SPMD_Root::SPMD_Root(SPMD_Tree * parent_) :
  SPMD_Tree(parent_)
{}

SPMD_Root::~SPMD_Root() {}

SPMD_NativeStmt::SPMD_NativeStmt(SPMD_Tree * parent_, SgStatement * stmt_) :
  SPMD_Tree(parent_),
  stmt(stmt_)
{}

SPMD_NativeStmt::~SPMD_NativeStmt() {}

SgStatement * SPMD_NativeStmt::getStatement() { return stmt; }

SPMD_Loop::SPMD_Loop(SPMD_Tree * parent_, RoseVariable iterator_, Bounds * bounds_, int increment_) :
  SPMD_Tree(parent_),
  iterator(iterator_),
  bounds(bounds_),
  increment(increment_)
{}

SPMD_Loop::~SPMD_Loop() {
  delete bounds;
}

RoseVariable & SPMD_Loop::getIterator() { return iterator; }

Bounds * SPMD_Loop::getBounds() { return bounds; }

int SPMD_Loop::getIncrement() { return increment; }

SPMD_DomainRestriction::SPMD_DomainRestriction(SPMD_Tree * parent_) :
  SPMD_Tree(parent_),
  restrictions()
{}

SPMD_DomainRestriction::~SPMD_DomainRestriction() {}

void SPMD_DomainRestriction::addRestriction(Expression * restriction) {
  restrictions.push_back(restriction);
}

std::vector<Expression *> & SPMD_DomainRestriction::getRestriction() { return restrictions; }

// first always survive...
SPMD_KernelCall::SPMD_KernelCall(SPMD_Tree * parent_, SPMD_Loop * first, SPMD_Loop * last) :
  SPMD_Tree(parent_),
  iterators(),
  restrictions()
{
  iterators.insert(std::pair<RoseVariable, Bounds *>(first->iterator, first->bounds->copy()));
  if (first != last) {
    iterators.insert(std::pair<RoseVariable, Bounds *>(last->iterator, last->bounds->copy()));
    SPMD_Tree * current = last->parent;
    while (current != first) {
      SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(current);
      SPMD_DomainRestriction * restriction = dynamic_cast<SPMD_DomainRestriction *>(current);
      if (loop != NULL) {
        iterators.insert(std::pair<RoseVariable, Bounds *>(loop->iterator, loop->bounds->copy()));
      }
      else if (restriction != NULL) {
        std::vector<Expression *>::iterator it_restrict;
        for (it_restrict = restriction->restrictions.begin(); it_restrict != restriction->restrictions.end(); it_restrict++)
          restrictions.push_back((*it_restrict)->copy());
      }
    }
  }

  std::vector<SPMD_Tree *>::iterator it = last->children.begin();
  while (it != children.end()) {
    appendChild(*it);
    it = last->children.erase(it);
  }
  SPMD_Tree * current = last;
  while (current != first) {
    if (current->children.size() == 0 && current->parent != NULL) {
      it = current->parent->children.begin();
      while (it != current->parent->children.end()) {
        if (*it == current) it = current->parent->children.erase(it);
        else it++;
      }
      SPMD_Tree * next = current->parent;
      delete current;
      current = next;
    }
    else break;
  }
}

SPMD_KernelCall::~SPMD_KernelCall() {}

SPMD_Comm::SPMD_Comm(SPMD_Tree * parent_, CommDescriptor * comm_descriptor_) :
  SPMD_Tree(parent_),
  comm_descriptor(comm_descriptor_)
{}

SPMD_Comm::~SPMD_Comm() {
  delete comm_descriptor;
}

SPMD_Sync::SPMD_Sync(SPMD_Tree * parent_, SyncDescriptor * sync_descriptor_) :
  SPMD_Tree(parent_),
  sync_descriptor(sync_descriptor_)
{}

SPMD_Sync::~SPMD_Sync() {
  delete sync_descriptor;
}

