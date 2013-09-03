#ifndef CFGUTILS_H
#define CFGUTILS_H

#include <set>
#include <string>
#include "widgets.h"

namespace fuse
{
  // Returns whether a given AST node that represents a constant is an integer and
  // sets *val to be the numeric value of that integer (all integer types are included
  // but not floating point, characters, etc.)
  bool IsConstInt(SgExpression* rhs, long &val);

  // pulls off all the SgCastExps that may be wrapping the given expression, returning the expression that is being wrapped
  SgExpression* unwrapCasts(SgExpression* e);

  // returns the DataflowNode that represents that start of the CFG of the given function's body
  CFGNode getFuncStartCFG(SgFunctionDefinition* func);

  // returns the DataflowNode that represents that end of the CFG of the given function's body
  CFGNode getFuncEndCFG(SgFunctionDefinition* func);

  // returns a string containing a unique name that is not otherwise used inside this project
  std::string genUniqueName();

  // returns the SgFunctionDeclaration for the function with the given name
  SgFunctionDeclaration* getFuncDecl(std::string name);

  // given a function's declaration, returns the function's definition.
  // handles the case where decl->get_definition()==NULL
  SgFunctionDefinition* funcDeclToDef(SgFunctionDeclaration* decl);
  
  // Returns a string representation of this node's key information
  std::string SgNode2Str(SgNode* sgn);

  // Returns a string representation of this CFG node's key information
  std::string CFGNode2Str(CFGNode n);

  // Returns a string representation of this CFG edge's key information
  std::string CFGEdge2Str(CFGEdge e);

  // Returns a string representation of this CFG paths's key information
  std::string CFGPath2Str(CFGPath p);

} // namespace fuse

#endif
