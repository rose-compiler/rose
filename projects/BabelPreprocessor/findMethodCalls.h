/*
 * File:        findMethodCalls.h
 * Copyright:   (c) 2005 The Regents of the University of California
 * Release:     $Name:  $
 * Revision:    @(#) $Revision: 1.2 $
 * Date:        $Date: 2008/01/08 02:55:50 $
 * Description: Define a 
 *
 */
#ifndef ROSE_BABEL_FINDMETHODCALLS_H
#define ROSE_BABEL_FINDMETHODCALLS_H

class SgNode;

/**
 * This attribute can be attached to SgFunctionDeclaration nodes to provide
 * a list of actual uses of the declared function.
 */
class MethodUseAttribute : public AstAttribute
{
 private:
  /**
   * This is a list of either SgFunctionCallExp or SgConstructorInitializer
   * nodes corresponding to actual invocations of a particular function.
   */
  Rose_STL_Container<SgNode*> d_uses;
 public:
  MethodUseAttribute(const Rose_STL_Container<SgNode*> &uses) : d_uses(uses) {}

  const Rose_STL_Container<SgNode*> get_uses() { return d_uses; }
};

class MethodSearchVisitor : public AstSimpleProcessing
{
 private:
  SgNode *d_root;
 public:
  MethodSearchVisitor(SgNode *root) : d_root(root) {}
  virtual void visit(SgNode* n);
};

#endif /* ROSE_BABEL_FINDMETHODCALLS_H */
