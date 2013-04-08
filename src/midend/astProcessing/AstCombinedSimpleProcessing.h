// Author: Gergo Barany
// $Id: AstCombinedSimpleProcessing.h,v 1.1 2008/01/08 02:56:38 dquinlan Exp $

// Class for combining AstSimpleProcessing traversals; see the comment in
// AstCombinedProcessing.h for general information.

#ifndef ASTCOMBINEDSIMPLEPROCESSING_H
#define ASTCOMBINEDSIMPLEPROCESSING_H

#include "AstSimpleProcessing.h"

class ROSE_DLL_API AstCombinedSimpleProcessing
    : public AstSimpleProcessing
{
public:
    typedef AstSimpleProcessing TraversalType;
    typedef TraversalType *TraversalPtr;
    typedef std::vector<TraversalPtr> TraversalPtrList;

    //! default constructor
    AstCombinedSimpleProcessing();
    //! constructor that makes an internal copy of an existing list of traversals
    AstCombinedSimpleProcessing(const TraversalPtrList &);

    //! simple function for adding a traversal to the internal list
    void addTraversal(TraversalPtr);
    //! function for obtaining a reference to the internal list of
    //! traversals, you can use this for any container operations you like
    //! (deleting elements etc.)
    TraversalPtrList &get_traversalPtrListRef();

protected:
    //! this method is called at every traversed node.
    virtual void visit(SgNode* astNode);
                                                                                                          
    virtual void atTraversalStart();
    virtual void atTraversalEnd();

    TraversalPtrList traversals;

private:
    TraversalPtrList::iterator tBegin, tEnd;
    TraversalPtrList::size_type numberOfTraversals;
};

class AstCombinedPrePostProcessing
    : public AstPrePostProcessing
{
public:
    typedef AstPrePostProcessing TraversalType;
    typedef TraversalType *TraversalPtr;
    typedef std::vector<TraversalPtr> TraversalPtrList;

    //! default constructor
    AstCombinedPrePostProcessing();
    //! constructor that makes an internal copy of an existing list of traversals
    AstCombinedPrePostProcessing(const TraversalPtrList &);

    //! simple function for adding a traversal to the internal list
    void addTraversal(TraversalPtr);
    //! function for obtaining a reference to the internal list of
    //! traversals, you can use this for any container operations you like
    //! (deleting elements etc.)
    TraversalPtrList &get_traversalPtrListRef();

protected:
    //! these methods are called at every traversed node.
    virtual void preOrderVisit(SgNode* astNode);
    virtual void postOrderVisit(SgNode* astNode);
                                                                                                          
    virtual void atTraversalStart();
    virtual void atTraversalEnd();

    TraversalPtrList traversals;

private:
    TraversalPtrList::iterator tBegin, tEnd;
    TraversalPtrList::size_type numberOfTraversals;
};

#endif
