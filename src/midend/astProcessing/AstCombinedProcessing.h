// Author: Gergo Barany
// $Id: AstCombinedProcessing.h,v 1.1 2008/01/08 02:56:38 dquinlan Exp $

// Classes for combining traversals. These allow you to evaluate the effects
// of multiple traversals (that do not modify the AST structure) in a single
// traversal. Each of the AstCombined*Processing classes is a subtype of the
// appropriate Ast*Processing class and contains a list of pointers to such
// traversals. Note that there are some restrictions on the types of
// traversals that can be grouped together: attributes must all be of the same
// pointer type (which may be a pointer to some common superclass).

// Do not assume that the traversals' visit functions are executed in any
// particular order. Do not introduce any other type of dependency between
// your traversals. In particular, do not modify the AST.

#ifndef ASTCOMBINEDPROCESSING_H
#define ASTCOMBINEDPROCESSING_H

#include "AstProcessing.h"

template <class InheritedAttributeType, class SynthesizedAttributeType>
class SgCombinedTreeTraversal
    : public SgTreeTraversal< std::vector<InheritedAttributeType> *, std::vector<SynthesizedAttributeType> *>
{
public:
    typedef SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType> TraversalType;
    typedef TraversalType *TraversalPtr;
    typedef std::vector<TraversalPtr> TraversalPtrList;
    typedef std::vector<InheritedAttributeType> InheritedAttributeTypeList;
    typedef std::vector<SynthesizedAttributeType> SynthesizedAttributeTypeList;
    typedef SgTreeTraversal<std::vector<InheritedAttributeType> *, std::vector<SynthesizedAttributeType> *> Superclass;
    typedef typename Superclass::SynthesizedAttributesList SynthesizedAttributesList;

    //! default constructor
    SgCombinedTreeTraversal();
    //! constructor that makes an internal copy of an existing list of traversals
    SgCombinedTreeTraversal(const TraversalPtrList &);

    //! simple function for adding a traversal to the internal list
    void addTraversal(TraversalPtr);
    //! function for obtaining a reference to the internal list of
    //! traversals, you can use this for any container operations you like
    //! (deleting elements etc.)
    TraversalPtrList &get_traversalPtrListRef();

protected:
    virtual InheritedAttributeTypeList *evaluateInheritedAttribute(
            SgNode *astNode,
            InheritedAttributeTypeList *inheritedValues);
    virtual SynthesizedAttributeTypeList *evaluateSynthesizedAttribute(
            SgNode *astNode,
            InheritedAttributeTypeList *inheritedValues,
            SynthesizedAttributesList synthesizedAttributes);
    virtual SynthesizedAttributeTypeList *defaultSynthesizedAttribute(InheritedAttributeTypeList *);
    virtual void atTraversalStart();
    virtual void atTraversalEnd();

    TraversalPtrList traversals;

private:
    typename TraversalPtrList::iterator tBegin, tEnd;
    typename TraversalPtrList::size_type numberOfTraversals;
};

template <class InheritedAttributeType, class SynthesizedAttributeType>
class AstCombinedTopDownBottomUpProcessing
    : public AstTopDownBottomUpProcessing<std::vector<InheritedAttributeType> *, std::vector<SynthesizedAttributeType> *>
{
public:
    typedef AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType> TraversalType;
    typedef TraversalType *TraversalPtr;
    typedef std::vector<TraversalPtr> TraversalPtrList;
    typedef std::vector<InheritedAttributeType> InheritedAttributeTypeList;
    typedef std::vector<SynthesizedAttributeType> SynthesizedAttributeTypeList;
    typedef AstTopDownBottomUpProcessing<std::vector<InheritedAttributeType> *, std::vector<SynthesizedAttributeType> *> Superclass;
    typedef typename Superclass::SynthesizedAttributesList SynthesizedAttributesList;

    //! default constructor
    AstCombinedTopDownBottomUpProcessing();
    //! constructor that makes an internal copy of an existing list of traversals
    AstCombinedTopDownBottomUpProcessing(const TraversalPtrList &);

    //! simple function for adding a traversal to the internal list
    void addTraversal(TraversalPtr);
    //! function for obtaining a reference to the internal list of
    //! traversals, you can use this for any container operations you like
    //! (deleting elements etc.)
    TraversalPtrList &get_traversalPtrListRef();

protected:
    virtual InheritedAttributeTypeList *evaluateInheritedAttribute(
            SgNode *astNode,
            InheritedAttributeTypeList *inheritedValues);
    virtual SynthesizedAttributeTypeList *evaluateSynthesizedAttribute(
            SgNode *astNode,
            InheritedAttributeTypeList *inheritedValues,
            SynthesizedAttributesList synthesizedAttributes);
    virtual SynthesizedAttributeTypeList *defaultSynthesizedAttribute(InheritedAttributeTypeList *);
    virtual void atTraversalStart();
    virtual void atTraversalEnd();

    TraversalPtrList traversals;

private:
    typename TraversalPtrList::iterator tBegin, tEnd;
    typename TraversalPtrList::size_type numberOfTraversals;
};

template <class InheritedAttributeType>
class AstCombinedTopDownProcessing
    : public AstTopDownProcessing<std::vector<InheritedAttributeType> *>
{
public:
    typedef AstTopDownProcessing<InheritedAttributeType> TraversalType;
    typedef TraversalType *TraversalPtr;
    typedef std::vector<TraversalPtr> TraversalPtrList;
    typedef std::vector<InheritedAttributeType> InheritedAttributeTypeList;

    //! default constructor
    AstCombinedTopDownProcessing();
    //! constructor that makes an internal copy of an existing list of traversals
    AstCombinedTopDownProcessing(const TraversalPtrList &);

    //! simple function for adding a traversal to the internal list
    void addTraversal(TraversalPtr);
    //! function for obtaining a reference to the internal list of
    //! traversals, zou can use this for any container operations you like
    //! (deleting elements etc.)
    TraversalPtrList &get_traversalPtrListRef();

protected:
    virtual InheritedAttributeTypeList *evaluateInheritedAttribute(
            SgNode *astNode,
            InheritedAttributeTypeList *inheritedValues);
    virtual void atTraversalStart();
    virtual void atTraversalEnd();
    virtual void destroyInheritedValue(SgNode*, InheritedAttributeTypeList *);

    TraversalPtrList traversals;

private:
    typename TraversalPtrList::iterator tBegin, tEnd;
    typename TraversalPtrList::size_type numberOfTraversals;
};

template <class SynthesizedAttributeType>
class AstCombinedBottomUpProcessing
    : public AstBottomUpProcessing<std::vector<SynthesizedAttributeType> *>
{
public:
    typedef AstBottomUpProcessing<SynthesizedAttributeType> TraversalType;
    typedef TraversalType *TraversalPtr;
    typedef std::vector<TraversalPtr> TraversalPtrList;
    typedef std::vector<SynthesizedAttributeType> SynthesizedAttributeTypeList;
    typedef AstBottomUpProcessing<SynthesizedAttributeTypeList *> Superclass;
    typedef typename Superclass::SynthesizedAttributesList SynthesizedAttributesList;

    //! default constructor
    AstCombinedBottomUpProcessing();
    //! constructor that makes an internal copy of an existing list of traversals
    AstCombinedBottomUpProcessing(const TraversalPtrList &);

    //! simple function for adding a traversal to the internal list
    void addTraversal(TraversalPtr);
    //! function for obtaining a reference to the internal list of
    //! traversals, you can use this for any container operations you like
    //! (deleting elements etc.)
    TraversalPtrList &get_traversalPtrListRef();

protected:
    virtual SynthesizedAttributeTypeList *evaluateSynthesizedAttribute(
            SgNode *astNode,
            SynthesizedAttributesList synthesizedAttributes);
    virtual SynthesizedAttributeTypeList *defaultSynthesizedAttribute();
    virtual void atTraversalStart();
    virtual void atTraversalEnd();

    TraversalPtrList traversals;

private:
    typename TraversalPtrList::iterator tBegin, tEnd;
    typename TraversalPtrList::size_type numberOfTraversals;
};

#include "AstCombinedProcessingImpl.h"

#include "AstCombinedSimpleProcessing.h"

#endif
