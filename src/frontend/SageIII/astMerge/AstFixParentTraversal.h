#ifndef _ASTFIXPARENTTRAVERSAL_H_
#define _ASTFIXPARENTTRAVERSAL_H_

//Simple, possibly unnecessary wrapper for SgNodes for use in the
//various parent traversals
class ParentAttribute {

public:

  ParentAttribute(SgNode * parent) : _parent(parent){}

  SgNode * get_parent() {return _parent;}
  void set_parent(SgNode * parent) {_parent = parent;}

private:
  SgNode * _parent;
};
  

//This traversal sets the parent pointers for a subtree appropriately
class AstFixParentTraversal : public AstTopDownProcessing<ParentAttribute> {

protected:
  virtual ParentAttribute evaluateInheritedAttribute (SgNode * node, ParentAttribute p);

};

#if 0
//This traversal checks that the parent points of a subtree are valid
class AstCheckParentTraversal : public AstTopDownProcessing<ParentAttribute> {

protected:
  virtual ParentAttribute evaluateInheritedAttribute (SgNode * node, ParentAttribute p);
};
#endif
#endif

