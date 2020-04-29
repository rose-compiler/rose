// Author: Gergo Barany
// $Id: AstSharedMemoryParallelProcessing.h,v 1.1 2008/01/08 02:56:39 dquinlan Exp $

// Classes for shared-memory (multithreaded) parallel AST traversals.

#ifndef ASTSHAREDMEMORYPARALLELPROCESSING_H
#define ASTSHAREDMEMORYPARALLELPROCESSING_H

#include "rosePublicConfig.h"

#include "AstProcessing.h"

// Class containing all the information needed to synchronize parallelizable traversals. All traversals running
// synchronously must have a shallow copy of this (they all need the same pointers!), the default copy mechanism takes
// care of that.
struct AstSharedMemoryParallelProcessingSynchronizationInfo
{
#ifdef _REENTRANT                                       // user wants mult-thread support? (e.g., g++ -pthread)?
# ifdef ROSE_HAVE_PTHREAD_H                             // POSIX threads are available?
    // mutex that controls access to the global stuff here
    pthread_mutex_t *mutex;
    // signal broadcast by last worker thread to arrive at a synchronization point
    pthread_cond_t *synchronizationEvent;
    // signal sent by workers when they exit
    pthread_cond_t *threadFinishedEvent;
# else
#  ifdef _MSC_VER
#   pragma message ("POSIX threads are unavailable on this platform");
#  else
#   warning "POSIX threads are unavailable on this platform."
#  endif
# endif
#endif

    // global counter of the number of threads that are still working (i.e.
    // have not sent a synchronizationEvent)
    size_t *workingThreads;
    // global counter of the number of threads that have completely finished
    // (i.e. have traversed the whole AST)
    size_t *finishedThreads;
    // number of nodes to be visited by each traversal before they are
    // supposed to synchronize
    size_t synchronizationWindowSize;

    AstSharedMemoryParallelProcessingSynchronizationInfo(size_t numberOfThreads, size_t synchronizationWindowSize);
    ~AstSharedMemoryParallelProcessingSynchronizationInfo();

#if 1
 // We can't implement this as private since it is required.  So we have a potential double free error here.
 // private:
 // DQ (9/13/2011): This copy constructor was built because static analysis tools (made it private to force compile time error if used).
    AstSharedMemoryParallelProcessingSynchronizationInfo( const AstSharedMemoryParallelProcessingSynchronizationInfo & X );
#endif
};

// Class containing the code needed for synchronization of parallelizable traversals. The parallelizable processing
// classes inherit privately from this because the code is the same for all of them.
class AstSharedMemoryParallelProcessingSynchronizationBase
{
protected:
    AstSharedMemoryParallelProcessingSynchronizationBase(const AstSharedMemoryParallelProcessingSynchronizationInfo &);
    // called when threads want to synchronize
    void synchronize();
    // called when threads are about to finish
    void signalFinish();

private:
    AstSharedMemoryParallelProcessingSynchronizationInfo syncInfo;
    size_t numberOfThreads;
};

// TOP DOWN BOTTOM UP parallel traversals

// Class representing a traversal that can run in parallel with some other instances of the same type. It is basically a
// combined processing class with a thin synchronization layer. The user will probably never need to instantiate this
// class, they should use the AstSharedMemoryParallel*Processing classes instead.
template <class InheritedAttributeType, class SynthesizedAttributeType>
class AstSharedMemoryParallelizableTopDownBottomUpProcessing
    : public AstCombinedTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>,
      private AstSharedMemoryParallelProcessingSynchronizationBase
{
public:
    typedef AstCombinedTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType> Superclass;
    typedef typename Superclass::TraversalType TraversalType;
    typedef typename Superclass::TraversalPtr TraversalPtr;
    typedef typename Superclass::TraversalPtrList TraversalPtrList;
    typedef typename Superclass::InheritedAttributeTypeList InheritedAttributeTypeList;
    typedef typename Superclass::SynthesizedAttributeTypeList SynthesizedAttributeTypeList;
    typedef typename Superclass::SynthesizedAttributesList SynthesizedAttributesList;

    AstSharedMemoryParallelizableTopDownBottomUpProcessing(
            const AstSharedMemoryParallelProcessingSynchronizationInfo &,
            const TraversalPtrList &);

    void set_runningParallelTraversal(bool val);

protected:
    virtual InheritedAttributeTypeList *evaluateInheritedAttribute(
            SgNode *astNode,
            InheritedAttributeTypeList *inheritedValues);
    virtual void atTraversalEnd();

private:
    size_t visitedNodes;
    bool runningParallelTraversal;
    size_t synchronizationWindowSize;
};

// Class for parallel execution of a number of traversals. This is a drop-in replacement for the corresponding
// AstCombined*Processing class, the usage is identical except that you call traverseInParallel() instead of traverse().
// (Calling traverse() is identical to AstCombined*Processing, i.e. it will not run in parallel.)
template <class InheritedAttributeType, class SynthesizedAttributeType>
class AstSharedMemoryParallelTopDownBottomUpProcessing
    : public AstCombinedTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>
{
public:
    typedef AstCombinedTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType> Superclass;
    typedef typename Superclass::InheritedAttributeTypeList InheritedAttributeTypeList;
    typedef typename Superclass::SynthesizedAttributeTypeList SynthesizedAttributeTypeList;
    typedef typename Superclass::TraversalPtr TraversalPtr;
    typedef typename Superclass::TraversalPtrList TraversalPtrList;

    typedef AstSharedMemoryParallelizableTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType> *ParallelizableTraversalPtr;
    typedef std::vector<ParallelizableTraversalPtr> ParallelizableTraversalPtrList;

    SynthesizedAttributeTypeList *traverseInParallel(SgNode *basenode,
            InheritedAttributeTypeList *inheritedValue);

    AstSharedMemoryParallelTopDownBottomUpProcessing();
    AstSharedMemoryParallelTopDownBottomUpProcessing(const TraversalPtrList &);

    void set_numberOfThreads(size_t threads);
    void set_synchronizationWindowSize(size_t windowSize);

private:
    size_t numberOfThreads;
    size_t synchronizationWindowSize;
};

// TOP DOWN parallel traversals

// Class representing a traversal that can run in parallel with some other instances of the same type. It is basically a
// combined processing class with a thin synchronization layer. The user will probably never need to instantiate this
// class, they should use the AstSharedMemoryParallel*Processing classes instead.
template <class InheritedAttributeType>
class AstSharedMemoryParallelizableTopDownProcessing
    : public AstCombinedTopDownProcessing<InheritedAttributeType>,
      private AstSharedMemoryParallelProcessingSynchronizationBase
{
public:
    typedef AstCombinedTopDownProcessing<InheritedAttributeType> Superclass;
    typedef typename Superclass::TraversalType TraversalType;
    typedef typename Superclass::TraversalPtr TraversalPtr;
    typedef typename Superclass::TraversalPtrList TraversalPtrList;
    typedef typename Superclass::InheritedAttributeTypeList InheritedAttributeTypeList;

    AstSharedMemoryParallelizableTopDownProcessing(
            const AstSharedMemoryParallelProcessingSynchronizationInfo &,
            const TraversalPtrList &);

    void set_runningParallelTraversal(bool val);

protected:
    virtual InheritedAttributeTypeList *evaluateInheritedAttribute(
            SgNode *astNode,
            InheritedAttributeTypeList *inheritedValues);
    virtual void atTraversalEnd();

private:
    size_t visitedNodes;
    bool runningParallelTraversal;
    size_t synchronizationWindowSize;
};

// Class for parallel execution of a number of traversals. This is a drop-in replacement for the corresponding
// AstCombined*Processing class, the usage is identical except that you call traverseInParallel() instead of traverse().
// (Calling traverse() is identical to AstCombined*Processing, i.e. it will not run in parallel.)
template <class InheritedAttributeType>
class AstSharedMemoryParallelTopDownProcessing
    : public AstCombinedTopDownProcessing<InheritedAttributeType>
{
public:
    typedef AstCombinedTopDownProcessing<InheritedAttributeType> Superclass;
    typedef typename Superclass::InheritedAttributeTypeList InheritedAttributeTypeList;
    typedef typename Superclass::TraversalPtr TraversalPtr;
    typedef typename Superclass::TraversalPtrList TraversalPtrList;

    typedef AstSharedMemoryParallelizableTopDownProcessing<InheritedAttributeType> *ParallelizableTraversalPtr;
    typedef std::vector<ParallelizableTraversalPtr> ParallelizableTraversalPtrList;

    void traverseInParallel(SgNode *basenode, InheritedAttributeTypeList *inheritedValue);

    AstSharedMemoryParallelTopDownProcessing();
    AstSharedMemoryParallelTopDownProcessing(const TraversalPtrList &);

    void set_numberOfThreads(size_t threads);
    void set_synchronizationWindowSize(size_t windowSize);

private:
    size_t numberOfThreads;
    size_t synchronizationWindowSize;
};

// BOTTOM UP parallel traversals

// Class representing a traversal that can run in parallel with some other instances of the same type. It is basically a
// combined processing class with a thin synchronization layer. The user will probably never need to instantiate this
// class, they should use the AstSharedMemoryParallel*Processing classes instead.
template <class SynthesizedAttributeType>
class AstSharedMemoryParallelizableBottomUpProcessing
    : public AstCombinedBottomUpProcessing<SynthesizedAttributeType>,
      private AstSharedMemoryParallelProcessingSynchronizationBase
{
public:
    typedef AstCombinedBottomUpProcessing<SynthesizedAttributeType> Superclass;
    typedef typename Superclass::TraversalType TraversalType;
    typedef typename Superclass::TraversalPtr TraversalPtr;
    typedef typename Superclass::TraversalPtrList TraversalPtrList;
    typedef typename Superclass::SynthesizedAttributeTypeList SynthesizedAttributeTypeList;
    typedef typename Superclass::SynthesizedAttributesList SynthesizedAttributesList;

    AstSharedMemoryParallelizableBottomUpProcessing(
            const AstSharedMemoryParallelProcessingSynchronizationInfo &,
            const TraversalPtrList &);

    void set_runningParallelTraversal(bool val);

protected:
    virtual SynthesizedAttributeTypeList *evaluateSynthesizedAttribute(
            SgNode *astNode,
            SynthesizedAttributesList synthesizedAttributes);
    virtual void atTraversalEnd();

private:
    size_t visitedNodes;
    bool runningParallelTraversal;
    size_t synchronizationWindowSize;
};

// Class for parallel execution of a number of traversals. This is a drop-in replacement for the corresponding
// AstCombined*Processing class, the usage is identical except that you call traverseInParallel() instead of traverse().
// (Calling traverse() is identical to AstCombined*Processing, i.e. it will not run in parallel.)
template <class SynthesizedAttributeType>
class AstSharedMemoryParallelBottomUpProcessing
    : public AstCombinedBottomUpProcessing<SynthesizedAttributeType>
{
public:
    typedef AstCombinedBottomUpProcessing<SynthesizedAttributeType> Superclass;
    typedef typename Superclass::SynthesizedAttributeTypeList SynthesizedAttributeTypeList;
    typedef typename Superclass::TraversalPtr TraversalPtr;
    typedef typename Superclass::TraversalPtrList TraversalPtrList;

    typedef AstSharedMemoryParallelizableBottomUpProcessing<SynthesizedAttributeType> *ParallelizableTraversalPtr;
    typedef std::vector<ParallelizableTraversalPtr> ParallelizableTraversalPtrList;

    SynthesizedAttributeTypeList *traverseInParallel(SgNode *basenode);

    AstSharedMemoryParallelBottomUpProcessing();
    AstSharedMemoryParallelBottomUpProcessing(const TraversalPtrList &);

    void set_numberOfThreads(size_t threads);
    void set_synchronizationWindowSize(size_t windowSize);

private:
    size_t numberOfThreads;
    size_t synchronizationWindowSize;
};

#include "AstSharedMemoryParallelProcessingImpl.h"

#include "AstSharedMemoryParallelSimpleProcessing.h"

#endif
