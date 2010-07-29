// Original Author (AstProcessing classes): Markus Schordan
// Rewritten by: Gergo Barany
// $Id: AstSimpleProcessing.h,v 1.3 2008/01/08 02:56:39 dquinlan Exp $

// See comments in AstProcessing.h for list of changes during the rewrite

#ifndef ASTPROCESSINGSIMPLE_H
#define ASTPROCESSINGSIMPLE_H

#include "AstProcessing.h"

// GB (7/6/2007): Added AstPrePostProcessing as a pre- and postorder
// traversal without attributes -- I feel this will be useful for many
// applications. (At least, I constantly find myself wanting to write
// traversals that simply push nodes onto a stack in the preorder visit and
// pop the stack in the postorder visit. With the existing traversal classes I
// had to use a TopDownBottomUpProcessing and ignore the attributes.)
class AstCombinedPrePostProcessing;

class AstPrePostProcessing
    : public SgTreeTraversal<DummyAttribute, DummyAttribute>
{
public:
    //! traverse the entire AST
    void traverse(SgNode *node);

    //! traverse only nodes which represent the same file as where the traversal was started
    void traverseWithinFile(SgNode *node);

    //! traverse only nodes which represent files which were specified on the command line (=input files).
    void traverseInputFiles(SgProject *projectNode);

    friend class AstCombinedPrePostProcessing;

protected:
    //! this method is called at every traversed node before its children are traversed
    virtual void preOrderVisit(SgNode *astNode) = 0;

    //! this method is called at every traversed node after its children were traversed
    virtual void postOrderVisit(SgNode *astNode) = 0;

    //! functions called when the traversal starts and ends, respectively
    virtual void atTraversalStart();
    virtual void atTraversalEnd();

private:
    DummyAttribute evaluateInheritedAttribute(SgNode *astNode, DummyAttribute inheritedValue);
    DummyAttribute evaluateSynthesizedAttribute(SgNode* astNode, DummyAttribute inheritedValue,
            SynthesizedAttributesList l);
    DummyAttribute defaultSynthesizedAttribute(DummyAttribute inheritedValue);
};


// Logically, AstSimpleProcessing could be derived from
// AstPrePostProcessing, but that results in a (barely) measurable
// performance hit.
class AstCombinedSimpleProcessing;

class AstSimpleProcessing
    : public SgTreeTraversal<DummyAttribute, DummyAttribute>
{
public:
    typedef t_traverseOrder Order;

    //! traverse the entire AST. Order defines preorder (preorder) or postorder (postorder) traversal. Default is 'preorder'.
    void traverse(SgNode* node, Order treeTraversalOrder);

    //! traverse only nodes which represent the same file as where the traversal was started
    void traverseWithinFile(SgNode* node, Order treeTraversalOrder);

    //! traverse only nodes which represent files which were specified on the command line (=input files).
    void traverseInputFiles(SgProject* projectNode, Order treeTraversalOrder);

    friend class AstCombinedSimpleProcessing;

protected:
    //! this method is called at every traversed node.
    virtual void visit(SgNode* astNode) = 0;

    //! GB (06/04/2007): A new virtual function called at the start of the
    //! traversal, before any node is actually visited; can be used to
    //! perform a "last-minute" computation of attributes that may have
    //! changed since the constructor was executed, but are constant
    //! during the traversal itself. A no-op by default.
    virtual void atTraversalStart();
    virtual void atTraversalEnd();

private:
    DummyAttribute evaluateInheritedAttribute(SgNode *astNode, DummyAttribute inheritedValue);
    DummyAttribute evaluateSynthesizedAttribute(SgNode* astNode, DummyAttribute inheritedValue,
            SynthesizedAttributesList l);
    DummyAttribute defaultSynthesizedAttribute(DummyAttribute inheritedValue);
};


/** @deprecated Use AstSimpleProcessing instead. (provided for compatibility with existing user code - will be removed at some point in future). */
class SgSimpleProcessing : public AstSimpleProcessing {};

#endif
