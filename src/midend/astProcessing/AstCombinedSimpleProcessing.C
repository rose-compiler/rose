

// Author: Gergo Barany
// $Id: AstCombinedSimpleProcessing.C,v 1.1 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTCOMBINEDSIMPLEPROCESSING_C
#define ASTCOMBINEDSIMPLEPROCESSING_C

#include "sage3basic.h"

#include "AstCombinedSimpleProcessing.h"

// combined simple traversals
AstCombinedSimpleProcessing::
AstCombinedSimpleProcessing()
    : traversals()
{
}

AstCombinedSimpleProcessing::
AstCombinedSimpleProcessing(
    const AstCombinedSimpleProcessing::TraversalPtrList &t)
    : traversals(t)
{
}

void
AstCombinedSimpleProcessing::
addTraversal(AstCombinedSimpleProcessing::TraversalPtr t)
{
    traversals.push_back(t);
}

AstCombinedSimpleProcessing::TraversalPtrList &
AstCombinedSimpleProcessing::
get_traversalPtrListRef()
{
    return traversals;
}

void
AstCombinedSimpleProcessing::
visit(SgNode *astNode)
{
    // Visit this node with each of the traversals.
    TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->visit(astNode);
}

void
AstCombinedSimpleProcessing::
atTraversalStart()
{
    // Compute some values that are constant during the traversal, saving
    // lots of method calls. We cannot reliably compute these earlier
    // because the user is allowed to mess with the traversal container
    // using the reference we provide.
    tBegin = traversals.begin();
    tEnd = traversals.end();
    numberOfTraversals = traversals.size();

    // Call this function for all traversals.
    TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalStart();
}

void
AstCombinedSimpleProcessing::
atTraversalEnd()
{
    // Call this function for all traversals.
    TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalEnd();
}

// combined pre-post visit traversals
AstCombinedPrePostProcessing::
AstCombinedPrePostProcessing()
    : traversals()
{
}

AstCombinedPrePostProcessing::
AstCombinedPrePostProcessing(
    const AstCombinedPrePostProcessing::TraversalPtrList &t)
    : traversals(t)
{
}

void
AstCombinedPrePostProcessing::
addTraversal(AstCombinedPrePostProcessing::TraversalPtr t)
{
    traversals.push_back(t);
}

AstCombinedPrePostProcessing::TraversalPtrList &
AstCombinedPrePostProcessing::
get_traversalPtrListRef()
{
    return traversals;
}

void
AstCombinedPrePostProcessing::
preOrderVisit(SgNode *astNode)
{
    // Visit this node with each of the traversals.
    TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->preOrderVisit(astNode);
}

void
AstCombinedPrePostProcessing::
postOrderVisit(SgNode *astNode)
{
    // Visit this node with each of the traversals.
    TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->postOrderVisit(astNode);
}

void
AstCombinedPrePostProcessing::
atTraversalStart()
{
    // Compute some values that are constant during the traversal, saving
    // lots of method calls. We cannot reliably compute these earlier
    // because the user is allowed to mess with the traversal container
    // using the reference we provide.
    tBegin = traversals.begin();
    tEnd = traversals.end();
    numberOfTraversals = traversals.size();

    // Call this function for all traversals.
    TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalStart();
}

void
AstCombinedPrePostProcessing::
atTraversalEnd()
{
    // Call this function for all traversals.
    TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalEnd();
}

#endif
