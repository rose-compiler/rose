// Author: Gergo Barany
// $Id: AstSharedMemoryParallelSimpleProcessing.h,v 1.1 2008/01/08 02:56:39 dquinlan Exp $

// Class for parallelizing AstSimpleProcessing traversals; see the comment in
// AstSharedMemoryParallelProcessing.h for general information.

#ifndef ASTSHAREDMEMORYPARALLELSIMPLEPROCESSING_H
#define ASTSHAREDMEMORYPARALLELSIMPLEPROCESSING_H

#include "AstSimpleProcessing.h"
#include "AstSharedMemoryParallelProcessing.h"

// parallel SIMPLE processing class

// Class representing a traversal that can run in parallel with some other instances of the same type. It is basically a
// combined processing class with a thin synchronization layer. The user will probably never need to instantiate this
// class, they should use the AstSharedMemoryParallel*Processing classes instead.
class AstSharedMemoryParallelizableSimpleProcessing
    : public AstCombinedSimpleProcessing,
      private AstSharedMemoryParallelProcessingSynchronizationBase
{
public:
    typedef AstCombinedSimpleProcessing Superclass;
    typedef Superclass::TraversalType TraversalType;
    typedef Superclass::TraversalPtr TraversalPtr;
    typedef Superclass::TraversalPtrList TraversalPtrList;

    AstSharedMemoryParallelizableSimpleProcessing(
            const AstSharedMemoryParallelProcessingSynchronizationInfo &,
            const TraversalPtrList &);

    void set_runningParallelTraversal(bool val);

protected:
    virtual void visit(SgNode *astNode);
    virtual void atTraversalEnd();

private:
    size_t visitedNodes;
    bool runningParallelTraversal;
    size_t synchronizationWindowSize;
};

// Class for parallel execution of a number of traversals. This is a drop-in
// replacement for the corresponding AstCombined*Processing class, the usage
// is identical except that you call traverseInParallel() instead of
// traverse(). (Calling traverse() is identical to AstCombined*Processing,
// i.e. it will not run in parallel.)
class ROSE_DLL_API AstSharedMemoryParallelSimpleProcessing
    : public AstCombinedSimpleProcessing
{
public:
    typedef AstCombinedSimpleProcessing Superclass;
    typedef Superclass::TraversalPtr TraversalPtr;
    typedef Superclass::TraversalPtrList TraversalPtrList;

    typedef AstSharedMemoryParallelizableSimpleProcessing *ParallelizableTraversalPtr;
    typedef std::vector<ParallelizableTraversalPtr> ParallelizableTraversalPtrList;

    AstSharedMemoryParallelSimpleProcessing(int threads);
    AstSharedMemoryParallelSimpleProcessing(const TraversalPtrList &, int threads);

    void traverseInParallel(SgNode *basenode, t_traverseOrder treeTraverseOrder);

private:
    size_t numberOfThreads;
    size_t synchronizationWindowSize;
};

// parallel PRE POST processing class

// Class representing a traversal that can run in parallel with some other instances of the same type. It is basically a
// combined processing class with a thin synchronization layer. The user will probably never need to instantiate this
// class, they should use the AstSharedMemoryParallel*Processing classes instead.
class AstSharedMemoryParallelizablePrePostProcessing
    : public AstCombinedPrePostProcessing,
      private AstSharedMemoryParallelProcessingSynchronizationBase
{
public:
    typedef AstCombinedPrePostProcessing Superclass;
    typedef Superclass::TraversalType TraversalType;
    typedef Superclass::TraversalPtr TraversalPtr;
    typedef Superclass::TraversalPtrList TraversalPtrList;

    AstSharedMemoryParallelizablePrePostProcessing(
            const AstSharedMemoryParallelProcessingSynchronizationInfo &,
            const TraversalPtrList &);

    void set_runningParallelTraversal(bool val);

protected:
    virtual void preOrderVisit(SgNode *astNode);
    virtual void atTraversalEnd();

private:
    size_t visitedNodes;
    bool runningParallelTraversal;
    size_t synchronizationWindowSize;
};

// Class for parallel execution of a number of traversals. This is a drop-in
// replacement for the corresponding AstCombined*Processing class, the usage
// is identical except that you call traverseInParallel() instead of
// traverse(). (Calling traverse() is identical to AstCombined*Processing,
// i.e. it will not run in parallel.)
class AstSharedMemoryParallelPrePostProcessing
    : public AstCombinedPrePostProcessing
{
public:
    typedef AstCombinedPrePostProcessing Superclass;
    typedef Superclass::TraversalPtr TraversalPtr;
    typedef Superclass::TraversalPtrList TraversalPtrList;

    typedef AstSharedMemoryParallelizablePrePostProcessing *ParallelizableTraversalPtr;
    typedef std::vector<ParallelizableTraversalPtr> ParallelizableTraversalPtrList;

    AstSharedMemoryParallelPrePostProcessing(int threads);
    AstSharedMemoryParallelPrePostProcessing(const TraversalPtrList &,int threads);

    void traverseInParallel(SgNode *basenode);

private:
    size_t numberOfThreads;
    size_t synchronizationWindowSize;
};

#endif
