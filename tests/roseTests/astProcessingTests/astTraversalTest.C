// Author: Gergo Barany
// $Id: astTraversalTest.C,v 1.1 2008/01/08 02:57:05 dquinlan Exp $

// Tests of the AST traversal mechanism.

#include <rose.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "AstSharedMemoryParallelProcessing.h"

#define OUTPUT_RESULTS 0

class NodeCountSimple: public AstSimpleProcessing
{
public:
    NodeCountSimple(enum VariantT variant)
      : variantCount(0), variant(variant)
    {
    }
 // setting this to false will use the old traversal successor container mechanism
    void set_useDefaultIndexBasedTraversal(bool flag)
    {
        AstSimpleProcessing::set_useDefaultIndexBasedTraversal(flag);
    }
    unsigned long variantCount;

protected:
    virtual void visit(SgNode *node)
    {
        if (variant == node->variantT())
            variantCount++;
    }
    VariantT variant;
};

class NodeCountPrePost: public AstPrePostProcessing
{
public:
    NodeCountPrePost(enum VariantT variant)
      : variantCount(0), variant(variant)
    {
    }
    unsigned long variantCount;

protected:
    virtual void preOrderVisit(SgNode *node)
    {
        if (variant == node->variantT())
            variantCount += 2;
    }
    virtual void postOrderVisit(SgNode *node)
    {
        if (variant == node->variantT())
            variantCount--;
    }
    VariantT variant;
};

class NodeCountBottomUp: public AstBottomUpProcessing<unsigned long *>
{
public:
    NodeCountBottomUp(enum VariantT variant)
      : variantCount(0), variant(variant)
    {
    }
    unsigned long variantCount;

protected:
    virtual unsigned long *evaluateSynthesizedAttribute(SgNode *node, SynthesizedAttributesList synAttributes)
    {
        unsigned long *count = defaultSynthesizedAttribute();
        std::vector<unsigned long *>::const_iterator s;

        if (variant == node->variantT())
            *count = 1;
        for (s = synAttributes.begin(); s != synAttributes.end(); ++s)
        {
            *count += **s;
            delete *s;
        }

        return count;
    }
    virtual unsigned long *defaultSynthesizedAttribute()
    {
        unsigned long *count = new unsigned long;

        *count = 0;

        return count;
    }
    VariantT variant;
};

class NodeCountTopDown: public AstTopDownProcessing<unsigned long *>
{
public:
    NodeCountTopDown(enum VariantT variant)
      : variantCount(0), variant(variant)
    {
    }
    unsigned long variantCount;

protected:
    virtual unsigned long *evaluateInheritedAttribute(SgNode *node, unsigned long *inhAttribute)
    {
        if (variant == node->variantT())
            ++*inhAttribute;

        return inhAttribute;
    }
    VariantT variant;
};

class NodeCountTopDownBottomUp: public AstTopDownBottomUpProcessing<unsigned long *, unsigned long *>
{
public:
    NodeCountTopDownBottomUp(enum VariantT variant)
      : variantCount(0), variant(variant)
    {
    }
    unsigned long variantCount;

protected:
    virtual unsigned long *evaluateInheritedAttribute(SgNode *, unsigned long *inhAttribute)
    {
        return inhAttribute;
    }
    virtual unsigned long *evaluateSynthesizedAttribute(SgNode *node, unsigned long *inhAttribute, SynthesizedAttributesList)
    {
        if (variant == node->variantT())
            (*inhAttribute)++;

        return inhAttribute;
    }
    virtual unsigned long *defaultSynthesizedAttribute(unsigned long *inhAttribute)
    {
        return inhAttribute;
    }
    VariantT variant;
};

double timeDifference(const struct timeval& end, const struct timeval& begin)
{
    return (end.tv_sec + end.tv_usec / 1.0e6) - (begin.tv_sec + begin.tv_usec / 1.0e6);
}

static inline timeval getCPUTime() {
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return ru.ru_utime;
}

template <class T>
std::vector<T *> *buildTraversalList()
{
    std::vector<T *> *traversalList = new std::vector<T *>;

    for (unsigned long i = 0; i < V_SgNumVariants; ++i)
        traversalList->push_back(new T(VariantT(i)));

    return traversalList;
}

void runSequentialTests(SgProject *root, std::vector<unsigned long> *referenceResults)
{
    struct timeval beginTime, endTime;
    size_t i;
    std::cout << "starting sequential tests" << std::endl;

    std::cout << "simple sequential using OLD traversal successor container mechanism" << std::endl;
    std::vector<NodeCountSimple *> *simpleList = buildTraversalList<NodeCountSimple>();
    std::cout << simpleList->size() << " traversals" << std::endl;
    std::vector<NodeCountSimple *>::iterator s;
    beginTime = getCPUTime();
    for (s = simpleList->begin(); s != simpleList->end(); ++s)
    {
        (*s)->set_useDefaultIndexBasedTraversal(false);
        (*s)->traverse(root, preorder);
    }
    endTime = getCPUTime();
    for (s = simpleList->begin(); s != simpleList->end(); ++s)
    {
#if OUTPUT_RESULTS
        std::cout << (*s)->variantCount << ' ';
#endif
        referenceResults->push_back((*s)->variantCount);
    }
#if OUTPUT_RESULTSY
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;
    delete simpleList;

    std::cout << "simple sequential using NEW index based access mechanism" << std::endl;
    std::vector<NodeCountSimple *> *simpleList2 = buildTraversalList<NodeCountSimple>();
    std::vector<NodeCountSimple *>::iterator s2;
    beginTime = getCPUTime();
    for (s2 = simpleList2->begin(); s2 != simpleList->end(); ++s2)
    {
        (*s2)->traverse(root, preorder);
    }
    endTime = getCPUTime();
    i = 0;
    for (s2 = simpleList->begin(); s2 != simpleList->end(); ++s2)
    {
#if OUTPUT_RESULTS
        std::cout << (*s2)->variantCount << ' ';
#endif
        ROSE_ASSERT((*s2)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;
    delete simpleList;

    std::cout << "pre-post sequential" << std::endl;
    std::vector<NodeCountPrePost *> *prePostList = buildTraversalList<NodeCountPrePost>();
    std::vector<NodeCountPrePost *>::iterator p;
    beginTime = getCPUTime();
    for (p = prePostList->begin(); p != prePostList->end(); ++p)
    {
        (*p)->traverse(root);
    }
    endTime = getCPUTime();
    i = 0;
    for (p = prePostList->begin(); p != prePostList->end(); ++p)
    {
#if OUTPUT_RESULTS
        std::cout << (*p)->variantCount << ' ';
#endif
        ROSE_ASSERT((*p)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;
    delete prePostList;

    std::cout << "bottom-up sequential" << std::endl;
    std::vector<NodeCountBottomUp *> *bottomUpList = buildTraversalList<NodeCountBottomUp>();
    std::vector<NodeCountBottomUp *>::iterator b;
    beginTime = getCPUTime();
    for (b = bottomUpList->begin(); b != bottomUpList->end(); ++b)
    {
        (*b)->variantCount = *(*b)->traverse(root);
    }
    endTime = getCPUTime();
    i = 0;
    for (b = bottomUpList->begin(); b != bottomUpList->end(); ++b)
    {
#if OUTPUT_RESULTS
        std::cout << (*b)->variantCount << ' ';
#endif
        ROSE_ASSERT((*b)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;
    delete bottomUpList;

    std::cout << "top-down sequential" << std::endl;
    std::vector<NodeCountTopDown *> *topDownList = buildTraversalList<NodeCountTopDown>();
    std::vector<NodeCountTopDown *>::iterator t;
    beginTime = getCPUTime();
    for (t = topDownList->begin(); t != topDownList->end(); ++t)
    {
        (*t)->traverse(root, &(*t)->variantCount);
    }
    endTime = getCPUTime();
    i = 0;
    for (t = topDownList->begin(); t != topDownList->end(); ++t)
    {
#if OUTPUT_RESULTS
        std::cout << (*t)->variantCount << ' ';
#endif
        ROSE_ASSERT((*t)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;
    delete topDownList;

    std::cout << "top-down bottom-up sequential" << std::endl;
    std::vector<NodeCountTopDownBottomUp *> *topDownBottomUpList = buildTraversalList<NodeCountTopDownBottomUp>();
    std::vector<NodeCountTopDownBottomUp *>::iterator tb;
    beginTime = getCPUTime();
    for (tb = topDownBottomUpList->begin(); tb != topDownBottomUpList->end(); ++tb)
    {
        (*tb)->variantCount = *(*tb)->traverse(root, &(*tb)->variantCount);
    }
    endTime = getCPUTime();
    i = 0;
    for (tb = topDownBottomUpList->begin(); tb != topDownBottomUpList->end(); ++tb)
    {
#if OUTPUT_RESULTS
        std::cout << (*tb)->variantCount << ' ';
#endif
        ROSE_ASSERT((*tb)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;
    delete topDownBottomUpList;
}

void runCombinedTests(SgProject *root, std::vector<unsigned long> *referenceResults)
{
    struct timeval beginTime, endTime;
    size_t i;
    std::cout << "starting combined tests" << std::endl;

    std::cout << "simple combined" << std::endl;
    std::vector<NodeCountSimple *> *simpleList = buildTraversalList<NodeCountSimple>();
    std::vector<NodeCountSimple *>::iterator s;
    AstCombinedSimpleProcessing combinedSimple;
    for (s = simpleList->begin(); s != simpleList->end(); ++s)
        combinedSimple.addTraversal(*s);
    beginTime = getCPUTime();
    combinedSimple.traverse(root, postorder);
    endTime = getCPUTime();
    i = 0;
    for (s = simpleList->begin(); s != simpleList->end(); ++s)
    {
#if OUTPUT_RESULTS
        std::cout << (*s)->variantCount << ' ';
#endif
        ROSE_ASSERT((*s)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;

    std::cout << "pre-post combined" << std::endl;
    std::vector<NodeCountPrePost *> *prePostList = buildTraversalList<NodeCountPrePost>();
    std::vector<NodeCountPrePost *>::iterator p;
    AstCombinedPrePostProcessing combinedPrePost;
    for (p = prePostList->begin(); p != prePostList->end(); ++p)
        combinedPrePost.addTraversal(*p);
    beginTime = getCPUTime();
    combinedPrePost.traverse(root);
    endTime = getCPUTime();
    i = 0;
    for (p = prePostList->begin(); p != prePostList->end(); ++p)
    {
#if OUTPUT_RESULTS
        std::cout << (*p)->variantCount << ' ';
#endif
        ROSE_ASSERT((*p)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;

    std::cout << "bottom-up combined" << std::endl;
    std::vector<NodeCountBottomUp *> *bottomUpList = buildTraversalList<NodeCountBottomUp>();
    std::vector<NodeCountBottomUp *>::iterator b;
    AstCombinedBottomUpProcessing<unsigned long *> combinedBottomUp;
    for (b = bottomUpList->begin(); b != bottomUpList->end(); ++b)
        combinedBottomUp.addTraversal(*b);
    beginTime = getCPUTime();
    std::vector<unsigned long *> *bottomUpResults = combinedBottomUp.traverse(root);
    endTime = getCPUTime();
    std::vector<unsigned long *>::const_iterator br;
    i = 0;
    for (br = bottomUpResults->begin(); br != bottomUpResults->end(); ++br)
    {
#if OUTPUT_RESULTS
        std::cout << **br << ' ';
#endif
        ROSE_ASSERT(**br == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;

    std::cout << "top-down combined" << std::endl;
    std::vector<NodeCountTopDown *> *topDownList = buildTraversalList<NodeCountTopDown>();
    std::vector<NodeCountTopDown *>::iterator t;
    AstCombinedTopDownProcessing<unsigned long *> combinedTopDown;
    std::vector<unsigned long *> variantCountPtrList;
    for (t = topDownList->begin(); t != topDownList->end(); ++t)
    {
        combinedTopDown.addTraversal(*t);
        variantCountPtrList.push_back(&(*t)->variantCount);
    }
    beginTime = getCPUTime();
    combinedTopDown.traverse(root, &variantCountPtrList);
    endTime = getCPUTime();
    i = 0;
    for (t = topDownList->begin(); t != topDownList->end(); ++t)
    {
#if OUTPUT_RESULTS
        std::cout << (*t)->variantCount << ' ';
#endif
        ROSE_ASSERT((*t)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;

    std::cout << "top-down bottom-up combined" << std::endl;
    std::vector<NodeCountTopDownBottomUp *> *topDownBottomUpList = buildTraversalList<NodeCountTopDownBottomUp>();
    std::vector<NodeCountTopDownBottomUp *>::iterator tb;
    AstCombinedTopDownBottomUpProcessing<unsigned long *, unsigned long *> combinedTopDownBottomUp;
    std::vector<unsigned long *> variantCountPtrList2;
    for (tb = topDownBottomUpList->begin(); tb != topDownBottomUpList->end(); ++tb)
    {
        combinedTopDownBottomUp.addTraversal(*tb);
        variantCountPtrList2.push_back(&(*tb)->variantCount);
    }
    beginTime = getCPUTime();
    std::vector<unsigned long *> *topDownBottomUpResults = combinedTopDownBottomUp.traverse(root, &variantCountPtrList2);
    endTime = getCPUTime();
    std::vector<unsigned long *>::const_iterator tbr;
    i = 0;
    for (tbr = topDownBottomUpResults->begin(); tbr != topDownBottomUpResults->end(); ++tbr)
    {
#if OUTPUT_RESULTS
        std::cout << **tbr << ' ';
#endif
        ROSE_ASSERT(**tbr == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;
}

void runParallelTests(SgProject *root, std::vector<unsigned long> *referenceResults)
{
#ifndef _REENTRANT                                      // Does user want multi-thread support? (e.g., g++ -pthread)
# ifdef _MSC_VER
#  pragma message("AstSharedMemoryParallelSimpleProcessing is not tested when the user has disabled parallel support")
# else
#  warning "AstSharedMemoryParalleSimpleProcessing is not tested when the user has disabled parallel support";
# endif
    std::cerr <<"AstSharedMemoryParallelSimpleProcessing is not tested when the user has disable parallel support.\n";
#else
    struct timeval beginTime, endTime;
    size_t i;
    std::cout << "starting shared memory parallel tests" << std::endl;

    std::cout << "simple parallel" << std::endl;
    std::vector<NodeCountSimple *> *simpleList = buildTraversalList<NodeCountSimple>();
    std::vector<NodeCountSimple *>::iterator s;
    AstSharedMemoryParallelSimpleProcessing parallelSimple(5);
    for (s = simpleList->begin(); s != simpleList->end(); ++s)
        parallelSimple.addTraversal(*s);
    beginTime = getCPUTime();
    parallelSimple.traverseInParallel(root, postorder);
    endTime = getCPUTime();
    i = 0;
    for (s = simpleList->begin(); s != simpleList->end(); ++s)
    {
#if OUTPUT_RESULTS
        std::cout << (*s)->variantCount << ' ';
#endif
        ROSE_ASSERT((*s)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;

    std::cout << "pre-post parallel" << std::endl;
    std::vector<NodeCountPrePost *> *prePostList = buildTraversalList<NodeCountPrePost>();
    std::vector<NodeCountPrePost *>::iterator p;
    AstSharedMemoryParallelPrePostProcessing parallelPrePost(5);
    for (p = prePostList->begin(); p != prePostList->end(); ++p)
        parallelPrePost.addTraversal(*p);
    beginTime = getCPUTime();
    parallelPrePost.traverseInParallel(root);
    endTime = getCPUTime();
    i = 0;
    for (p = prePostList->begin(); p != prePostList->end(); ++p)
    {
#if OUTPUT_RESULTS
        std::cout << (*p)->variantCount << ' ';
#endif
        ROSE_ASSERT((*p)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;

    std::cout << "bottom-up parallel" << std::endl;
    std::vector<NodeCountBottomUp *> *bottomUpList = buildTraversalList<NodeCountBottomUp>();
    std::vector<NodeCountBottomUp *>::iterator b;
    AstSharedMemoryParallelBottomUpProcessing<unsigned long *> parallelBottomUp;
    for (b = bottomUpList->begin(); b != bottomUpList->end(); ++b)
        parallelBottomUp.addTraversal(*b);
    beginTime = getCPUTime();
    std::vector<unsigned long *> *bottomUpResults = parallelBottomUp.traverseInParallel(root);
    endTime = getCPUTime();
    std::vector<unsigned long *>::const_iterator br;
    i = 0;
    for (br = bottomUpResults->begin(); br != bottomUpResults->end(); ++br)
    {
#if OUTPUT_RESULTS
        std::cout << **br << ' ';
#endif
        ROSE_ASSERT(**br == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;

    std::cout << "top-down parallel" << std::endl;
    std::vector<NodeCountTopDown *> *topDownList = buildTraversalList<NodeCountTopDown>();
    std::vector<NodeCountTopDown *>::iterator t;
    AstSharedMemoryParallelTopDownProcessing<unsigned long *> parallelTopDown;
    std::vector<unsigned long *> variantCountPtrList;
    for (t = topDownList->begin(); t != topDownList->end(); ++t)
    {
        parallelTopDown.addTraversal(*t);
        variantCountPtrList.push_back(&(*t)->variantCount);
    }
    beginTime = getCPUTime();
    parallelTopDown.traverseInParallel(root, &variantCountPtrList);
    endTime = getCPUTime();
    i = 0;
    for (t = topDownList->begin(); t != topDownList->end(); ++t)
    {
#if OUTPUT_RESULTS
        std::cout << (*t)->variantCount << ' ';
#endif
        ROSE_ASSERT((*t)->variantCount == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;

    std::cout << "top-down bottom-up parallel" << std::endl;
    std::vector<NodeCountTopDownBottomUp *> *topDownBottomUpList = buildTraversalList<NodeCountTopDownBottomUp>();
    std::vector<NodeCountTopDownBottomUp *>::iterator tb;
    AstSharedMemoryParallelTopDownBottomUpProcessing<unsigned long *, unsigned long *> parallelTopDownBottomUp;
    std::vector<unsigned long *> variantCountPtrList2;
    for (tb = topDownBottomUpList->begin(); tb != topDownBottomUpList->end(); ++tb)
    {
        parallelTopDownBottomUp.addTraversal(*tb);
        variantCountPtrList2.push_back(&(*tb)->variantCount);
    }
    beginTime = getCPUTime();
    std::vector<unsigned long *> *topDownBottomUpResults = parallelTopDownBottomUp.traverseInParallel(root, &variantCountPtrList2);
    endTime = getCPUTime();
    std::vector<unsigned long *>::const_iterator tbr;
    i = 0;
    for (tbr = topDownBottomUpResults->begin(); tbr != topDownBottomUpResults->end(); ++tbr)
    {
#if OUTPUT_RESULTS
        std::cout << **tbr << ' ';
#endif
        ROSE_ASSERT(**tbr == referenceResults->at(i++));
    }
#if OUTPUT_RESULTS
    std::cout << std::endl;
#endif
    std::cout << "approximate time (seconds): " << timeDifference(endTime, beginTime) << std::endl;
#endif
}

class NodeCounterTraversal: public AstSimpleProcessing
{
public:
    unsigned long count;
    NodeCounterTraversal(): count(0) { }

protected:
    virtual void visit(SgNode *node) { count++; }
};

int main(int argc, char **argv)
{
    struct timeval beginTime, endTime;

    std::cout << "starting frontend" << std::endl;
    beginTime = getCPUTime();
    SgProject *root = frontend(argc, argv);
    endTime = getCPUTime();
    std::cout << "frontend ran for about " << timeDifference(endTime, beginTime) << " seconds on "
              << root->numberOfFiles() << " files" << std::endl;

    NodeCounterTraversal counter;
    counter.traverse(root, preorder);
    std::cout << "AST has " << counter.count << " nodes" << std::endl;

    std::vector<unsigned long> referenceResults;
    runSequentialTests(root, &referenceResults);
    std::cout << std::endl;
    runCombinedTests(root, &referenceResults);
    std::cout << std::endl;
    runParallelTests(root, &referenceResults);
    std::cout << std::endl;

    return backend(root);
}
