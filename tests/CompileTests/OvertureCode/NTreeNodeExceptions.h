#ifndef NTREE_NODE_EXCEPTIONS_H
#define NTREE_NODE_EXCEPTIONS_H

#include "AssertException.h"
#include "AbstractException.h"

// exceptions for NTreeNode

class NTreeNodeError : public AbstractException 
{

public:
  virtual void debug_print() const { cerr<<"\nNTreeNode Error"; }

};

class InvalidNode : public NTreeNodeError
{

public:
  void debug_print() const 
  { 
    NTreeNodeError::debug_print();
    cerr<<": InvalidNode : Tree node failed internal validity check";
  }
};

class TreeDegreeViolation : public NTreeNodeError
{
public:
  void debug_print() const
  {
    NTreeNodeError::debug_print();
    cerr<<": TreeDegreeViolation : attempting to manipulate non-existant node";
  }
};
 
class NodeFullError : public NTreeNodeError
{
public:
  void debug_print() const
  {
    NTreeNodeError::debug_print();
    cerr<<": NodeFullError : attempting to add items to an already full node";
  }
};

#endif
