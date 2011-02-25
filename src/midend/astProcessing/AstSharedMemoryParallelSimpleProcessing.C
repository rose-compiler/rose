// Author: Gergo Barany
// $Id: AstSharedMemoryParallelSimpleProcessing.C,v 1.1 2008/01/08 02:56:39 dquinlan Exp $
#include "sage3basic.h"
#ifdef _MSC_VER
#pragma message ("Error: pthread.h is unavailable on MSVC, we might want to use boost.thread library.")
#else
#include <pthread.h>
#endif



#include "AstSharedMemoryParallelSimpleProcessing.h"

// general stuff
AstSharedMemoryParallelProcessingSynchronizationInfo::AstSharedMemoryParallelProcessingSynchronizationInfo(
        size_t numberOfThreads, size_t synchronizationWindowSize)
    : synchronizationWindowSize(synchronizationWindowSize)
{
    mutex = new pthread_mutex_t;
    pthread_mutex_init(mutex, NULL);

    synchronizationEvent = new pthread_cond_t;
    pthread_cond_init(synchronizationEvent, NULL);

    threadFinishedEvent = new pthread_cond_t;
    pthread_cond_init(threadFinishedEvent, NULL);

    workingThreads = new size_t;
    *workingThreads = numberOfThreads;

    finishedThreads = new size_t;
    *finishedThreads = 0;
}

AstSharedMemoryParallelProcessingSynchronizationInfo::~AstSharedMemoryParallelProcessingSynchronizationInfo()
{
    pthread_mutex_destroy(mutex);
    delete mutex;

    pthread_cond_destroy(synchronizationEvent);
    delete synchronizationEvent;

    pthread_cond_destroy(threadFinishedEvent);
    delete threadFinishedEvent;

    delete workingThreads;
    delete finishedThreads;
}

AstSharedMemoryParallelProcessingSynchronizationBase::AstSharedMemoryParallelProcessingSynchronizationBase(
        const AstSharedMemoryParallelProcessingSynchronizationInfo &syncInfo)
    : syncInfo(syncInfo), numberOfThreads(*syncInfo.workingThreads)
{
}

void AstSharedMemoryParallelProcessingSynchronizationBase::synchronize()
{
    // (Safely) decrement the shared variable counting the threads that are
    // still working, then wait until that value is 0 (i.e. all threads are
    // done) and we are signalled to go ahead.
    pthread_mutex_lock(syncInfo.mutex);
    *syncInfo.workingThreads -= 1;
    if (*syncInfo.workingThreads == 0)
    {
        // If the counter reached 0, this is the last thread to get here; tell
        // the others to wake up and then just go ahead.
        *syncInfo.workingThreads = numberOfThreads;
        pthread_cond_broadcast(syncInfo.synchronizationEvent);
    }
    else
    {
        // Not all threads are done yet, wait for them to get to this
        // synchronization point.
        pthread_cond_wait(syncInfo.synchronizationEvent, syncInfo.mutex);
        // This loop guards against spurious wake ups which are apparently
        // allowed by the pthreads standard under some circumstances. I have
        // never observed them, but this doesn't cost anything, so why not
        // have it here.
        while (*syncInfo.workingThreads == 0)
        {
            std::cout << this << ": spurious wake up" << std::endl;
            pthread_cond_wait(syncInfo.synchronizationEvent, syncInfo.mutex);
        }
    }
    pthread_mutex_unlock(syncInfo.mutex);
}

void AstSharedMemoryParallelProcessingSynchronizationBase::signalFinish()
{
    pthread_mutex_lock(syncInfo.mutex);
    *syncInfo.finishedThreads += 1;
    pthread_cond_signal(syncInfo.threadFinishedEvent);
    pthread_mutex_unlock(syncInfo.mutex);
}

// parallel SIMPLE implementation

AstSharedMemoryParallelizableSimpleProcessing::AstSharedMemoryParallelizableSimpleProcessing(
        const AstSharedMemoryParallelProcessingSynchronizationInfo &syncInfo,
        const AstSharedMemoryParallelizableSimpleProcessing::TraversalPtrList &t)
    : AstCombinedSimpleProcessing(t), AstSharedMemoryParallelProcessingSynchronizationBase(syncInfo),
      visitedNodes(0), runningParallelTraversal(false),
      synchronizationWindowSize(syncInfo.synchronizationWindowSize)
{
}

void AstSharedMemoryParallelizableSimpleProcessing::set_runningParallelTraversal(bool val)
{
    runningParallelTraversal = val;
}

void AstSharedMemoryParallelizableSimpleProcessing::visit(SgNode *astNode)
{
    if (runningParallelTraversal && ++visitedNodes > synchronizationWindowSize)
    {
        synchronize();
        visitedNodes = 0;
    }

    // let the superclass handle the actual visits
    Superclass::visit(astNode);
}

void AstSharedMemoryParallelizableSimpleProcessing::atTraversalEnd()
{
    // delegate call to superclass
    Superclass::atTraversalEnd();

    if (runningParallelTraversal)
    {
        signalFinish();
        // clear the flag so subsequent traversals can be non-parallel
        runningParallelTraversal = false;
    }
}

// This class holds the arguments to a traversal thread: The parallelizable
// traversal class that contains a number of traversals, the node to start the
// traversal from, and the traversal order.
struct AstSharedMemoryParallelSimpleThreadArgs
{
    AstSharedMemoryParallelizableSimpleProcessing *traversal;
    SgNode *basenode;
    t_traverseOrder treeTraverseOrder;

    AstSharedMemoryParallelSimpleThreadArgs(AstSharedMemoryParallelizableSimpleProcessing *traversal, SgNode *basenode, t_traverseOrder treeTraverseOrder)
        : traversal(traversal), basenode(basenode), treeTraverseOrder(treeTraverseOrder)
    {
    }
};

// This is the function that is executed in each thread. It basically unpacks
// the args object and starts the traversal specified in that. Synchronization
// is built into the parallelizable processing classes.
void *parallelSimpleProcessingThread(void *p)
{
    AstSharedMemoryParallelSimpleThreadArgs *threadArgs = (AstSharedMemoryParallelSimpleThreadArgs *) p;

    AstSharedMemoryParallelizableSimpleProcessing *traversal = threadArgs->traversal;
    SgNode *basenode = threadArgs->basenode;
    t_traverseOrder treeTraverseOrder = threadArgs->treeTraverseOrder;
    delete threadArgs;

    // Set the flag that indicates that this is indeed a parallel traversal;
    // it is cleared by the traversal class itself when it is done.
    traversal->set_runningParallelTraversal(true);
    // Start the traversal.
    traversal->traverse(basenode, treeTraverseOrder);

    return NULL;
}

AstSharedMemoryParallelSimpleProcessing::AstSharedMemoryParallelSimpleProcessing(int threads)
    : AstCombinedSimpleProcessing(), numberOfThreads(threads), synchronizationWindowSize(100000)
{
}

AstSharedMemoryParallelSimpleProcessing::AstSharedMemoryParallelSimpleProcessing(const AstSharedMemoryParallelSimpleProcessing::TraversalPtrList &t,int threads)
    : AstCombinedSimpleProcessing(t), numberOfThreads(threads), synchronizationWindowSize(100000)
{
}

void AstSharedMemoryParallelSimpleProcessing::traverseInParallel(SgNode *basenode,
        t_traverseOrder treeTraverseOrder)
{
    size_t numberOfTraversals = traversals.size();
    size_t i;

    AstSharedMemoryParallelProcessingSynchronizationInfo syncInfo(numberOfThreads, synchronizationWindowSize);

    // Chop the flat list of traversals apart and distribute them into a few
    // parallelizable traversals.
    //    std::cerr << "AstSharedMemoryParallelSimpleProcessing : starting " << numberOfThreads << " threads. " << std::endl;
    ParallelizableTraversalPtrList parallelTraversals(numberOfThreads);
    size_t begin = 0, end;
    for (i = 0; i < numberOfThreads; i++)
    {
        end = begin + numberOfTraversals / numberOfThreads + 1;
        if (end > numberOfTraversals)
            end = numberOfTraversals;

        parallelTraversals[i]
            = new AstSharedMemoryParallelizableSimpleProcessing(syncInfo,
                    std::vector<TraversalPtr>(traversals.begin() + begin, traversals.begin() + end));
        begin = end;
    }

    // Start a thread for each of the parallelizable traversals with its share
    // of the initial inherited attributes.
    pthread_t *threads = new pthread_t[numberOfThreads];
    for (i = 0; i < numberOfThreads; i++)
    {
        pthread_create(&threads[i], NULL,
                parallelSimpleProcessingThread,
                new AstSharedMemoryParallelSimpleThreadArgs(parallelTraversals[i], basenode, treeTraverseOrder));
        //      std::cerr << " starting " << i << " thread. " << std::endl;
    }

    // Main "event loop" for the "master" thread: Simply wait for the
    // condition that is signalled when a thread is completely done with its
    // traversal. The counter tells us when we are finished.
    pthread_mutex_lock(syncInfo.mutex);
    while (*syncInfo.finishedThreads < numberOfThreads)
        pthread_cond_wait(syncInfo.threadFinishedEvent, syncInfo.mutex);
    pthread_mutex_unlock(syncInfo.mutex);

    // Join all threads. (Strictly speaking this is not necessary since we
    // know that they are all done, but it doesn't really const anything).
    void *resultDummy;
    for (i = 0; i < numberOfThreads; i++)
        pthread_join(threads[i], &resultDummy);
    delete threads;
    // Done!
}

// parallel PRE POST implementation

AstSharedMemoryParallelizablePrePostProcessing::AstSharedMemoryParallelizablePrePostProcessing(
        const AstSharedMemoryParallelProcessingSynchronizationInfo &syncInfo,
        const AstSharedMemoryParallelizablePrePostProcessing::TraversalPtrList &t)
    : AstCombinedPrePostProcessing(t), AstSharedMemoryParallelProcessingSynchronizationBase(syncInfo),
      visitedNodes(0), runningParallelTraversal(false),
      synchronizationWindowSize(syncInfo.synchronizationWindowSize)
{
}

void AstSharedMemoryParallelizablePrePostProcessing::set_runningParallelTraversal(bool val)
{
    runningParallelTraversal = val;
}

void AstSharedMemoryParallelizablePrePostProcessing::preOrderVisit(SgNode *astNode)
{
    if (runningParallelTraversal && ++visitedNodes > synchronizationWindowSize)
    {
        synchronize();
        visitedNodes = 0;
    }

    // let the superclass handle the actual visits
    Superclass::preOrderVisit(astNode);
}

void AstSharedMemoryParallelizablePrePostProcessing::atTraversalEnd()
{
    // delegate call to superclass
    Superclass::atTraversalEnd();

    if (runningParallelTraversal)
    {
        signalFinish();
        // clear the flag so subsequent traversals can be non-parallel
        runningParallelTraversal = false;
    }
}

// This class holds the arguments to a traversal thread: The parallelizable
// traversal class that contains a number of traversals and the node to start
// the traversal from.
struct AstSharedMemoryParallelPrePostThreadArgs
{
    AstSharedMemoryParallelizablePrePostProcessing *traversal;
    SgNode *basenode;

    AstSharedMemoryParallelPrePostThreadArgs(AstSharedMemoryParallelizablePrePostProcessing *traversal, SgNode *basenode)
        : traversal(traversal), basenode(basenode)
    {
    }
};

// This is the function that is executed in each thread. It basically unpacks
// the args object and starts the traversal specified in that. Synchronization
// is built into the parallelizable processing classes.
void *parallelPrePostProcessingThread(void *p)
{
    AstSharedMemoryParallelPrePostThreadArgs *threadArgs = (AstSharedMemoryParallelPrePostThreadArgs *) p;

    AstSharedMemoryParallelizablePrePostProcessing *traversal = threadArgs->traversal;
    SgNode *basenode = threadArgs->basenode;
    delete threadArgs;

    // Set the flag that indicates that this is indeed a parallel traversal;
    // it is cleared by the traversal class itself when it is done.
    traversal->set_runningParallelTraversal(true);
    // Start the traversal.
    traversal->traverse(basenode);

    return NULL;
}

AstSharedMemoryParallelPrePostProcessing::AstSharedMemoryParallelPrePostProcessing(int threads)
    : AstCombinedPrePostProcessing(), numberOfThreads(threads), synchronizationWindowSize(100000)
{
}

AstSharedMemoryParallelPrePostProcessing::AstSharedMemoryParallelPrePostProcessing(const AstSharedMemoryParallelPrePostProcessing::TraversalPtrList &t,int threads)
    : AstCombinedPrePostProcessing(t), numberOfThreads(threads), synchronizationWindowSize(100000)
{
}

void AstSharedMemoryParallelPrePostProcessing::traverseInParallel(SgNode *basenode)
{
    size_t numberOfTraversals = traversals.size();
    size_t i;

    AstSharedMemoryParallelProcessingSynchronizationInfo syncInfo(numberOfThreads, synchronizationWindowSize);

    // Chop the flat list of traversals apart and distribute them into a few
    // parallelizable traversals.
    ParallelizableTraversalPtrList parallelTraversals(numberOfThreads);
    size_t begin = 0, end;
    for (i = 0; i < numberOfThreads; i++)
    {
        end = begin + numberOfTraversals / numberOfThreads + 1;
        if (end > numberOfTraversals)
            end = numberOfTraversals;

        parallelTraversals[i]
            = new AstSharedMemoryParallelizablePrePostProcessing(syncInfo,
                    std::vector<TraversalPtr>(traversals.begin() + begin, traversals.begin() + end));
        begin = end;
    }

    // Start a thread for each of the parallelizable traversals with its share
    // of the initial inherited attributes.
    pthread_t *threads = new pthread_t[numberOfThreads];
    for (i = 0; i < numberOfThreads; i++)
    {
        pthread_create(&threads[i], NULL,
                parallelPrePostProcessingThread,
                new AstSharedMemoryParallelPrePostThreadArgs(parallelTraversals[i], basenode));
    }

    // Main "event loop" for the "master" thread: Simply wait for the
    // condition that is signalled when a thread is completely done with its
    // traversal. The counter tells us when we are finished.
    pthread_mutex_lock(syncInfo.mutex);
    while (*syncInfo.finishedThreads < numberOfThreads)
        pthread_cond_wait(syncInfo.threadFinishedEvent, syncInfo.mutex);
    pthread_mutex_unlock(syncInfo.mutex);

    // Join all threads. (Strictly speaking this is not necessary since we
    // know that they are all done, but it doesn't really const anything).
    void *resultDummy;
    for (i = 0; i < numberOfThreads; i++)
        pthread_join(threads[i], &resultDummy);
    delete threads;
    // Done!
}
