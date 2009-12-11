/*!
 *  \file PrePostTraversal.cc
 *
 *  \brief Implements a top-down/bottom-up traversal without explicit
 *  attributes, i.e., implements a variation on the simple traversal
 *  that exposes the top-down and bottom-up visits.
 */
#include <rose.h>
#include "PrePostTraversal.hh"

_DummyAttribute
ASTtools::PrePostTraversal::evaluateInheritedAttribute (SgNode* node,
                                                        _DummyAttribute d)
{
  visitTopDown (node);
  return d;
}

_DummyAttribute
ASTtools::PrePostTraversal::evaluateSynthesizedAttribute (SgNode* node,
                                                          _DummyAttribute d,
                                                          SynthesizedAttributesList)
{
  visitBottomUp (node);
  return d;
}

void
ASTtools::PrePostTraversal::traverse (SgNode* node)
{
  AstTopDownBottomUpProcessing<_DummyAttribute, _DummyAttribute>::traverse (node, _DummyAttribute ());
}

void
ASTtools::PrePostTraversal::traverseWithinFile (SgNode* node)
{
  AstTopDownBottomUpProcessing<_DummyAttribute, _DummyAttribute>::traverseWithinFile (node, _DummyAttribute ());
}

// eof
