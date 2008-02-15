#ifndef _CREATESLICE_H_
#define _CREATESLICE_H_

/*! \class CreateSlice

  This class is a traversal which marks elements that should be in a
  slice. It does so by accepting a list of nodes that belong in the
  slice. Currently, it's basically a destructive operation: the ast
  itself is changed to produce the correct slice.

  @todo The way that we create the slice is rather... simple. We
  completely destroy any attendant file info by simply marking
  everything as compiler generated and then marking the nodes of the
  slice as to be output. It may be better in the future to create a
  second AST containing just the nodes of the slice.
*/
#include "rose.h"
#include <set>

class CreateSlice : public AstTopDownBottomUpProcessing<bool, bool> {

public:

  CreateSlice(std::set<SgNode *> & saveNodes) : _toSave(saveNodes) {}

  //bool traverse(SgNode * node) {return traverse(node, false);}
  bool traverse(SgNode * node) {
    return AstTopDownBottomUpProcessing<bool, bool>::traverse(node, false);
  }
protected:

  virtual bool evaluateInheritedAttribute(SgNode * node, bool partOfSlice);
  
  virtual bool evaluateSynthesizedAttribute(SgNode * node, bool inherited, SubTreeSynthesizedAttributes atts);
  virtual bool defaultSynthesizedAttribute() {return false;}

private:

  std::set<SgNode *> _toSave;

};

#endif
