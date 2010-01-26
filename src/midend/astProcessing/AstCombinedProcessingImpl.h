// Author: Gergo Barany
// $Id: AstCombinedProcessing.C,v 1.1 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTCOMBINEDPROCESSING_C
#define ASTCOMBINEDPROCESSING_C

//#include "sage3.h"

#include "AstCombinedProcessing.h"

// Throughout this file, I is the InheritedAttributeType, S is the
// SynthesizedAttributeType -- the type names are still horrible

// combined TREE TRAVERSAL implementation -- you will probably only want to
// use this to combine traversals of different types
template <class I, class S>
SgCombinedTreeTraversal<I, S>::
SgCombinedTreeTraversal()
    : traversals()
{
}

template <class I, class S>
SgCombinedTreeTraversal<I, S>::
SgCombinedTreeTraversal(
            const typename SgCombinedTreeTraversal<I, S>::TraversalPtrList &t)
    : traversals(t)
{
}

template <class I, class S>
void
SgCombinedTreeTraversal<I, S>::
addTraversal(typename SgCombinedTreeTraversal<I, S>::TraversalPtr t)
{
    traversals.push_back(t);
}

template <class I, class S>
typename SgCombinedTreeTraversal<I, S>::TraversalPtrList &
SgCombinedTreeTraversal<I, S>::
get_traversalPtrListRef()
{
    return traversals;
}

template <class I, class S>
typename SgCombinedTreeTraversal<I, S>
    ::InheritedAttributeTypeList *
SgCombinedTreeTraversal<I, S>::
evaluateInheritedAttribute(SgNode *astNode,
        typename SgCombinedTreeTraversal<I, S>
        ::InheritedAttributeTypeList *inheritedValues)
{
    InheritedAttributeTypeList *result
        = new InheritedAttributeTypeList();
    // Reserve just enough space for one inherited attribute per
    // traversal, this keeps us from doing expensive resizing later.
    result->reserve(numberOfTraversals);

    // Fill the list by evaluating the inherited attributes for each
    // traversal.
    typename TraversalPtrList::iterator t = tBegin;
    typename InheritedAttributeTypeList::iterator i
        = inheritedValues->begin();
    typename InheritedAttributeTypeList::const_iterator iEnd
        = inheritedValues->end();

    while (t != tEnd && i != iEnd)
        result->push_back((*t++)->evaluateInheritedAttribute(astNode, *i++));
    ROSE_ASSERT(t == tEnd && i == iEnd);
    ROSE_ASSERT(result->size() == numberOfTraversals);

    // The inherited attribute list allocated here should be deleted in the
    // evaluateSynthesizedAttribute() function.
    return result;
}

template <class I, class S>
typename SgCombinedTreeTraversal<I, S>
    ::SynthesizedAttributeTypeList *
SgCombinedTreeTraversal<I, S>::
evaluateSynthesizedAttribute(SgNode *astNode,
        typename SgCombinedTreeTraversal<I, S>
        ::InheritedAttributeTypeList *inheritedValues,
        typename SgCombinedTreeTraversal<I, S>
        ::SynthesizedAttributesList synthesizedAttributes)
{
    // Let N = number of traversals, M = number of node successors.
    // synthesizedAttributes is a list of M lists of N attributes each; we
    // want to call each of the N traversals with the corresponding M
    // synthesized attributes.
    // This is one of the obscure functions.

    typename SynthesizedAttributesList::size_type M
        = synthesizedAttributes.size();
    // Create a container of size M in which we will store the synthesized
    // attributes for each traversal.
    typename TraversalType::SynthesizedAttributesList
        attributesForTraversal(M);
    // Create a list for the traversal results.
    SynthesizedAttributeTypeList *result
        = new SynthesizedAttributeTypeList();
    result->reserve(numberOfTraversals);

    typename TraversalPtrList::size_type i;
    typename SynthesizedAttributesList::size_type j;

    for (i = 0; i < numberOfTraversals; i++)
    {
        for (j = 0; j < M; j++)
            attributesForTraversal[j] = (*synthesizedAttributes[j])[i];

        result->push_back(
                traversals[i]->evaluateSynthesizedAttribute(
                    astNode,
                    (*inheritedValues)[i],
                    attributesForTraversal));
    }

    // The lists of synthesized attributes passed to us are not needed
    // anymore, free them.
    for (j = 0; j < M; j++)
        delete synthesizedAttributes[j];

    // inheritedValues is a pointer to a container that is dynamically
    // allocated in evaluateInheritedAttribute(). Now that all successor
    // nodes have been visited, we can free the allocated memory.
    delete inheritedValues;

    return result;
}

template <class I, class S>
typename SgCombinedTreeTraversal<I, S>
    ::SynthesizedAttributeTypeList *
SgCombinedTreeTraversal<I, S>::
defaultSynthesizedAttribute(typename SgCombinedTreeTraversal<I, S>
        ::InheritedAttributeTypeList *inheritedValues)
{
    // Create a list for the default attributes.
    SynthesizedAttributeTypeList *result
        = new SynthesizedAttributeTypeList(numberOfTraversals);

#if 1
    typename TraversalPtrList::size_type i;
    for (i = 0; i < numberOfTraversals; i++)
        (*result)[i]
            = traversals[i]->defaultSynthesizedAttribute((*inheritedValues)[i]);
#else
    typename TraversalPtrList::iterator i = traversals.begin();
    typename SynthesizedAttributeTypeList::iterator j
        = result->begin();
    typename InheritedAttributeTypPtrList::iterator k
        = inheritedValues->begin();
    while (i != tEnd)
        *j++ = (*i++)->defaultSynthesizedAttribute(*k++);
#endif

    return result;
}

template <class I, class S>
void
SgCombinedTreeTraversal<I, S>::
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
    typename TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalStart();
}

template <class I, class S>
void
SgCombinedTreeTraversal<I, S>::
atTraversalEnd()
{
    // Call this function for all traversals.
    typename TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalEnd();
}

// combined TOP DOWN BOTTOM UP implementation

template <class I, class S>
AstCombinedTopDownBottomUpProcessing<I, S>::
AstCombinedTopDownBottomUpProcessing()
    : traversals()
{
}

template <class I, class S>
AstCombinedTopDownBottomUpProcessing<I, S>::
AstCombinedTopDownBottomUpProcessing(
        const typename AstCombinedTopDownBottomUpProcessing<I, S>::TraversalPtrList &t)
    : traversals(t)
{
}

template <class I, class S>
void
AstCombinedTopDownBottomUpProcessing<I, S>::
addTraversal(typename AstCombinedTopDownBottomUpProcessing<I, S>::TraversalPtr t)
{
    traversals.push_back(t);
}

template <class I, class S>
typename AstCombinedTopDownBottomUpProcessing<I, S>::TraversalPtrList &
AstCombinedTopDownBottomUpProcessing<I, S>::
get_traversalPtrListRef()
{
    return traversals;
}

template <class I, class S>
typename AstCombinedTopDownBottomUpProcessing<I, S>
    ::InheritedAttributeTypeList *
AstCombinedTopDownBottomUpProcessing<I, S>::
evaluateInheritedAttribute(SgNode *astNode,
        typename AstCombinedTopDownBottomUpProcessing<I, S>
        ::InheritedAttributeTypeList *inheritedValues)
{
    InheritedAttributeTypeList *result
        = new InheritedAttributeTypeList();
    // Reserve just enough space for one inherited attribute per
    // traversal, this keeps us from doing expensive resizing later.
    result->reserve(numberOfTraversals);

    // Fill the list by evaluating the inherited attributes for each
    // traversal.
    typename TraversalPtrList::iterator t = tBegin;
    typename InheritedAttributeTypeList::iterator i
        = inheritedValues->begin();
    typename InheritedAttributeTypeList::const_iterator iEnd
        = inheritedValues->end();

    while (t != tEnd && i != iEnd)
        result->push_back((*t++)->evaluateInheritedAttribute(astNode, *i++));
    ROSE_ASSERT(t == tEnd && i == iEnd);
    ROSE_ASSERT(result->size() == numberOfTraversals);

    // The inherited attribute list allocated here should be deleted in the
    // evaluateSynthesizedAttribute() function.
    return result;
}

template <class I, class S>
typename AstCombinedTopDownBottomUpProcessing<I, S>
    ::SynthesizedAttributeTypeList *
AstCombinedTopDownBottomUpProcessing<I, S>::
evaluateSynthesizedAttribute(SgNode *astNode,
        typename AstCombinedTopDownBottomUpProcessing<I, S>
        ::InheritedAttributeTypeList *inheritedValues,
        typename AstCombinedTopDownBottomUpProcessing<I, S>
        ::SynthesizedAttributesList synthesizedAttributes)
{
    // Let N = number of traversals, M = number of node successors.
    // synthesizedAttributes is a list of M lists of N attributes each; we
    // want to call each of the N traversals with the corresponding M
    // synthesized attributes.
    // This is one of the obscure functions.

    typename SynthesizedAttributesList::size_type M
        = synthesizedAttributes.size();
    // Create a container of size M in which we will store the synthesized
    // attributes for each traversal.
    typename TraversalType::SynthesizedAttributesList
        attributesForTraversal(M);
    // Create a list for the traversal results.
    SynthesizedAttributeTypeList *result
        = new SynthesizedAttributeTypeList();
    result->reserve(numberOfTraversals);

    typename TraversalPtrList::size_type i;
    typename SynthesizedAttributesList::size_type j;

    for (i = 0; i < numberOfTraversals; i++)
    {
        for (j = 0; j < M; j++)
            attributesForTraversal[j] = (*synthesizedAttributes[j])[i];

        result->push_back(
                traversals[i]->evaluateSynthesizedAttribute(
                    astNode,
                    (*inheritedValues)[i],
                    attributesForTraversal));
    }

    // The lists of synthesized attributes passed to us are not needed
    // anymore, free them.
    for (j = 0; j < M; j++)
        delete synthesizedAttributes[j];

    // inheritedValues is a pointer to a container that is dynamically
    // allocated in evaluateInheritedAttribute(). Now that all successor
    // nodes have been visited, we can free the allocated memory.
    delete inheritedValues;

    return result;
}

template <class I, class S>
typename AstCombinedTopDownBottomUpProcessing<I, S>
    ::SynthesizedAttributeTypeList *
AstCombinedTopDownBottomUpProcessing<I, S>::
defaultSynthesizedAttribute(typename AstCombinedTopDownBottomUpProcessing<I, S>
        ::InheritedAttributeTypeList *inheritedValues)
{
    // Create a list for the default attributes.
    SynthesizedAttributeTypeList *result
        = new SynthesizedAttributeTypeList(numberOfTraversals);

    typename TraversalPtrList::size_type i;
    for (i = 0; i < numberOfTraversals; i++)
        (*result)[i]
            = traversals[i]->defaultSynthesizedAttribute((*inheritedValues)[i]);

    return result;
}

template <class I, class S>
void
AstCombinedTopDownBottomUpProcessing<I, S>::
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
    typename TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalStart();
}

template <class I, class S>
void
AstCombinedTopDownBottomUpProcessing<I, S>::
atTraversalEnd()
{
    // Call this function for all traversals.
    typename TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalEnd();
}


// combined TOP DOWN implementation

template <class I>
AstCombinedTopDownProcessing<I>::
AstCombinedTopDownProcessing()
    : traversals()
{
}

template <class I>
AstCombinedTopDownProcessing<I>::
AstCombinedTopDownProcessing(
        const typename AstCombinedTopDownProcessing<I>::TraversalPtrList &t)
    : traversals(t)
{
}

template <class I>
void
AstCombinedTopDownProcessing<I>::
addTraversal(typename AstCombinedTopDownProcessing<I>::TraversalPtr t)
{
    traversals.push_back(t);
}

template <class I>
typename AstCombinedTopDownProcessing<I>::TraversalPtrList &
AstCombinedTopDownProcessing<I>::
get_traversalPtrListRef()
{
    return traversals;
}

template <class I>
typename AstCombinedTopDownProcessing<I>
    ::InheritedAttributeTypeList *
AstCombinedTopDownProcessing<I>::
evaluateInheritedAttribute(SgNode *astNode,
        typename AstCombinedTopDownProcessing<I>
        ::InheritedAttributeTypeList *inheritedValues)
{
    InheritedAttributeTypeList *result
        = new InheritedAttributeTypeList();
    // Reserve just enough space for one inherited attribute per
    // traversal, this keeps us from doing expensive resizing later.
    result->reserve(numberOfTraversals);

    // Fill the list by evaluating the inherited attributes for each
    // traversal.
    typename TraversalPtrList::iterator t = tBegin;
    typename InheritedAttributeTypeList::iterator i
        = inheritedValues->begin();
    typename InheritedAttributeTypeList::const_iterator iEnd
        = inheritedValues->end();

    while (t != tEnd && i != iEnd)
        result->push_back((*t++)->evaluateInheritedAttribute(astNode, *i++));
    ROSE_ASSERT(t == tEnd && i == iEnd);
    ROSE_ASSERT(result->size() == numberOfTraversals);

    // The list of inherited attributes allocated here should be freed in
    // the destroyInheritedAttribute() function.
    return result;
}

template <class I>
void
AstCombinedTopDownProcessing<I>::
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
    typename TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalStart();
}

template <class I>
void
AstCombinedTopDownProcessing<I>::
atTraversalEnd()
{
    // Call this function for all traversals.
    typename TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalEnd();
}

template <class I>
void
AstCombinedTopDownProcessing<I>::
destroyInheritedValue(SgNode *node, typename AstCombinedTopDownProcessing<I>::
        InheritedAttributeTypeList *inheritedValues)
{
    typename TraversalPtrList::iterator t = tBegin;
    typename InheritedAttributeTypeList::iterator i
        = inheritedValues->begin();
    typename InheritedAttributeTypeList::const_iterator iEnd
        = inheritedValues->end();

    // Call this function for all traversals.
    while (t != tEnd && i != iEnd)
        (*t++)->destroyInheritedValue(node, *i++);
    ROSE_ASSERT(t == tEnd && i == iEnd);

    // inheritedValues is a pointer to a container that is dynamically
    // allocated in evaluateInheritedAttribute(). Now that all successor
    // nodes have been visited, we can free the allocated memory.
    delete inheritedValues;
}

// combined BOTTOM UP implementation

template <class S>
AstCombinedBottomUpProcessing<S>::
AstCombinedBottomUpProcessing()
    : traversals()
{
}

template <class S>
AstCombinedBottomUpProcessing<S>::
AstCombinedBottomUpProcessing(
        const typename AstCombinedBottomUpProcessing<S>::TraversalPtrList &t)
    : traversals(t)
{
}

template <class S>
void
AstCombinedBottomUpProcessing<S>::
addTraversal(typename AstCombinedBottomUpProcessing<S>::TraversalPtr t)
{
    traversals.push_back(t);
}

template <class S>
typename AstCombinedBottomUpProcessing<S>::TraversalPtrList &
AstCombinedBottomUpProcessing<S>::
get_traversalPtrListRef()
{
    return traversals;
}

template <class S>
typename AstCombinedBottomUpProcessing<S>
    ::SynthesizedAttributeTypeList *
AstCombinedBottomUpProcessing<S>::
evaluateSynthesizedAttribute(SgNode *astNode,
        typename AstCombinedBottomUpProcessing<S>
        ::SynthesizedAttributesList synthesizedAttributes)
{
    // Let N = number of traversals, M = number of node successors.
    // synthesizedAttributes is a list of M lists of N attributes each; we
    // want to call each of the N traversals with the corresponding M
    // synthesized attributes.
    // This is one of the obscure functions.

    typename SynthesizedAttributesList::size_type M
        = synthesizedAttributes.size();
    // Create a container of size M in which we will store the synthesized
    // attributes for each traversal.
    typename TraversalType::SynthesizedAttributesList
        attributesForTraversal(M, NULL);
    // Create a list for the traversal results.
    SynthesizedAttributeTypeList *result
        = new SynthesizedAttributeTypeList(numberOfTraversals);

    typename TraversalPtrList::size_type i;
    typename SynthesizedAttributesList::size_type j;

    for (i = 0; i < numberOfTraversals; i++)
    {
        for (j = 0; j < M; j++)
            attributesForTraversal[j] = (*synthesizedAttributes[j])[i];

        (*result)[i]
            = traversals[i]->evaluateSynthesizedAttribute(
                    astNode,
                    attributesForTraversal);
    }
    ROSE_ASSERT(result->size() == numberOfTraversals);

    // The lists of synthesized attributes passed to us are not needed
    // anymore, free them.
    for (j = 0; j < M; j++)
        delete synthesizedAttributes[j];

    return result;
}

template <class S>
typename AstCombinedBottomUpProcessing<S>
    ::SynthesizedAttributeTypeList *
AstCombinedBottomUpProcessing<S>::
defaultSynthesizedAttribute()
{
    // Create a list for the default attributes.
    SynthesizedAttributeTypeList *result
        = new SynthesizedAttributeTypeList(numberOfTraversals);

    typename TraversalPtrList::size_type i;
    for (i = 0; i < numberOfTraversals; i++)
        (*result)[i] = traversals[i]->defaultSynthesizedAttribute();

    return result;
}

template <class S>
void
AstCombinedBottomUpProcessing<S>::
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
    typename TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalStart();
}

template <class S>
void
AstCombinedBottomUpProcessing<S>::
atTraversalEnd()
{
    // Call this function for all traversals.
    typename TraversalPtrList::iterator t;
    for (t = tBegin; t != tEnd; ++t)
        (*t)->atTraversalEnd();
}

#endif
