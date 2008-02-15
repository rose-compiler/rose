// Distributed memory traversal implementation.
// Author: Gergo Barany
// $Id: DistributedMemoryAnalysisTemplateDefs.C,v 1.1 2008/01/08 02:55:52 dquinlan Exp $

// DQ (9/28/2007): Comment by Gergo to explain nesting problem:
// The current implementation of DistributedMemoryAnalysisPreTraversal has an issue within
// AST node types that may be nested within constructs of the same type (basic blocks within
// basic blocks, function definitions within function definitions etc.). When entering the
// first scope in the top-down traversal, it is recorded at the end of the funcDecls list.
// The bottom-up pass (in the destroyInheritedValue function) must check that the node it
// is invoked with is actually that last element of funcDecls -- only then can the inFunc
// flag be unset and the number of nodes recorded.
// (In case this doesn't work, complain to gergo@llnl.gov.)


#ifndef DISTRIBUTED_MEMORY_ANALYSIS_TEMPLATE_DEFS_C
#define DISTRIBUTED_MEMORY_ANALYSIS_TEMPLATE_DEFS_C

#include <mpi.h>
#include "DistributedMemoryAnalysis.h"

#define DEBUG_OUTPUT false

// class DistributedMemoryAnalysisBase

template <class InheritedAttributeType>
bool
DistributedMemoryAnalysisBase<InheritedAttributeType>::
isRootProcess() const
{
    return (my_rank == root_process);
}

template <class InheritedAttributeType>
int
DistributedMemoryAnalysisBase<InheritedAttributeType>::
myID() const
{
    return my_rank;
}

template <class InheritedAttributeType>
int
DistributedMemoryAnalysisBase<InheritedAttributeType>::
numberOfProcesses() const
{
    return processes;
}

template <class InheritedAttributeType>
std::pair<int, int>
DistributedMemoryAnalysisBase<InheritedAttributeType>::
computeFunctionIndices(
        SgNode *root,
        InheritedAttributeType rootInheritedValue,
        AstTopDownProcessing<InheritedAttributeType> *preTraversal)
{
 // This function load balances the work assigned to processors.  Work is currently
 // assigned to processors on the basis SgFunctionDeclaration (if it is a defining declaration).

    DistributedMemoryAnalysisPreTraversal<InheritedAttributeType> nodeCounter(preTraversal);

    nodeCounter.traverse(root, rootInheritedValue);

    funcDecls = nodeCounter.get_funcDecls();
    initialInheritedValues = nodeCounter.get_initialInheritedValues();
    std::vector<size_t> &nodeCounts = nodeCounter.get_nodeCounts();
    ROSE_ASSERT(funcDecls.size() == initialInheritedValues.size());
    ROSE_ASSERT(funcDecls.size() == nodeCounts.size());

    std::vector<size_t>::const_iterator itr;
    size_t totalNodes = 0;
    for (itr = nodeCounts.begin(); itr != nodeCounts.end(); ++itr)
        totalNodes += *itr;
    if (DistributedMemoryAnalysisBase<InheritedAttributeType>::myID() == 0)
    {
#if DEBUG_OUTPUT
        std::cout << "functions: " << funcDecls.size() << ", total nodes: " << totalNodes << std::endl;
#endif
    }

    size_t lo, hi = 0;
    size_t nodesSoFar = 0, nodesOfPredecessors = 0;
    size_t my_lo, my_hi;
    functionsPerProcess.clear();
    for (int rank = 0; rank < processes; rank++)
    {
        const size_t myNodesHigh = (totalNodes / processes + 1) * (rank + 1);
        // set lower limit
        lo = hi;
        nodesOfPredecessors = nodesSoFar;
        size_t myNodes = nodesOfPredecessors + nodeCounts[hi];
        // find upper limit
        for (hi = lo + 1; hi < funcDecls.size(); hi++)
        {
            if (myNodes > myNodesHigh)
                break;
            else if (myNodesHigh - myNodes < nodeCounts[hi] / 2)
                break;
            myNodes += nodeCounts[hi];
        }
        nodesSoFar = myNodes;
        functionsPerProcess.push_back(hi - lo);

        // make sure all files have been assigned to some process
        if (rank == processes - 1)
        {
            if (hi != funcDecls.size())
            {
                std::cerr << "hi: " << hi << ", funcDecls.size(): " << funcDecls.size() << std::endl;
                std::cerr << "It looks like you asked me to analyze too few functions in too many processes."
                    << std::endl << " Please give me fewer processes or a larger program to work on." << std::endl;
            }
            ROSE_ASSERT(hi == funcDecls.size());
        }

        if (rank == my_rank)
        {
            my_lo = lo;
            my_hi = hi;
#if DEBUG_OUTPUT
            std::cout << "process " << rank << ": functions [" << lo << "," << hi
                << "[ for a total of "
                << myNodes - nodesOfPredecessors
                << " nodes" << std::endl;
#endif
        }
    }

    return std::make_pair(my_lo, my_hi);
}

template <class InheritedAttributeType>
DistributedMemoryAnalysisBase<InheritedAttributeType>::
DistributedMemoryAnalysisBase()
{
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
}

template <class InheritedAttributeType>
DistributedMemoryAnalysisBase<InheritedAttributeType>::
~DistributedMemoryAnalysisBase()
{
}

// class DistributedMemoryTraversal

template <class InheritedAttributeType, class SynthesizedAttributeType>
void
DistributedMemoryTraversal<InheritedAttributeType, SynthesizedAttributeType>::
performAnalysis(SgNode *root, InheritedAttributeType rootInheritedValue,
                AstTopDownProcessing<InheritedAttributeType> *preTraversal,
                AstBottomUpProcessing<SynthesizedAttributeType> *postTraversal)
{
    /* see what functions to run our analysis on */
    std::pair<int, int> my_limits = computeFunctionIndices(root, rootInheritedValue, preTraversal);

    /* run the analysis on the defining function declarations found above and store the serialized results */
    std::vector<std::pair<int, void *> > serializedResults;
    for (int i = my_limits.first; i < my_limits.second; i++)
    {
        SynthesizedAttributeType result =
            analyzeSubtree(DistributedMemoryAnalysisBase<InheritedAttributeType>::funcDecls[i],
                           DistributedMemoryAnalysisBase<InheritedAttributeType>::initialInheritedValues[i]);
        serializedResults.push_back(serializeAttribute(result));
    }

    /* compute how much total memory our attributes take up, and concatenate the serialized attributes into a single
     * buffer */
    size_t functions = DistributedMemoryAnalysisBase<InheritedAttributeType>::funcDecls.size();
    int myFunctionsPerProcess = DistributedMemoryAnalysisBase<InheritedAttributeType>::functionsPerProcess[
                                    DistributedMemoryAnalysisBase<InheritedAttributeType>::myID()];
    int *myStateSizes = new int[myFunctionsPerProcess];
    int myTotalSize = 0;
    for (int i = 0; i < myFunctionsPerProcess; i++)
    {
        myStateSizes[i] = serializedResults[i].first;
        myTotalSize += myStateSizes[i];
    }
    unsigned char *myBuffer = new unsigned char[myTotalSize];
    int sizeSoFar = 0;
    for (int i = 0; i < myFunctionsPerProcess; i++)
    {
        std::memcpy(myBuffer + sizeSoFar, serializedResults[i].second, serializedResults[i].first);
        sizeSoFar += serializedResults[i].first;
     // now that we have made a copy of the serialized attribute, we can free the user-allocated memory
        deleteSerializedAttribute(serializedResults[i]);
    }

    /* communicate results: first, gather the sizes of the respective states into an array */
    int *displacements = new int[DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses()];
    displacements[0] = 0;
    for (int i = 1; i < DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses(); i++)
    {
     // Displacements are in units of data elements, not bytes
        displacements[i] = displacements[i-1] + DistributedMemoryAnalysisBase<InheritedAttributeType>::functionsPerProcess[i-1];
    }
    int *stateSizes = new int[functions];
    MPI_Allgatherv(myStateSizes, myFunctionsPerProcess, MPI_INT,
                   stateSizes, &DistributedMemoryAnalysisBase<InheritedAttributeType>::functionsPerProcess[0], displacements, MPI_INT,
                   MPI_COMM_WORLD);

    /* from the state sizes communicated above, compute the total buffer size
     * for all concatenated states, and the indices where each part starts */
    int totalSize = 0;
    int *totalStateSizes = new int[DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses()];
    int j = 0;
    for (int i = 0; i < DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses(); i++)
    {
        displacements[i] = totalSize;
        totalStateSizes[i] = 0;
        int j_lim = j + DistributedMemoryAnalysisBase<InheritedAttributeType>::functionsPerProcess[i];
        while (j < j_lim)
            totalStateSizes[i] += stateSizes[j++];
        totalSize += totalStateSizes[i];
    }

    /* communicate results: gather the actual state information, concatenating
     * it into recvbuf on each process */
    unsigned char *recvbuf = new unsigned char[totalSize];
    MPI_Allgatherv(myBuffer, myTotalSize, MPI_UNSIGNED_CHAR,
                   recvbuf, totalStateSizes, displacements, MPI_UNSIGNED_CHAR,
                   MPI_COMM_WORLD);

    /* unpack the serialized states and store them away for the post traversal to use */
    functionResults.clear();
    j = 0;
    for (int i = 0; i < DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses(); i++)
    {
        int j_lim = j + DistributedMemoryAnalysisBase<InheritedAttributeType>::functionsPerProcess[i];
        int sizeSoFar = 0;
        while (j < j_lim)
        {
            std::pair<int, void *> serializedAttribute
                = std::make_pair(stateSizes[j], recvbuf + displacements[i] + sizeSoFar);
            SynthesizedAttributeType attribute = deserializeAttribute(serializedAttribute);
            functionResults.push_back(attribute);
            sizeSoFar += stateSizes[j];
            j++;
        }
    }

    /* perform the post traversal */
    DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType> postT(postTraversal, functionResults);
    finalResults = postT.traverse(root, false);

    /* clean up */
    delete[] myStateSizes;
    delete[] myBuffer;
    delete[] stateSizes;
    delete[] totalStateSizes;
    delete[] displacements;
    delete[] recvbuf;
}

template <class InheritedAttributeType, class SynthesizedAttributeType>
DistributedMemoryTraversal<InheritedAttributeType, SynthesizedAttributeType>::
DistributedMemoryTraversal()
{
}

template <class InheritedAttributeType, class SynthesizedAttributeType>
DistributedMemoryTraversal<InheritedAttributeType, SynthesizedAttributeType>::
~DistributedMemoryTraversal()
{
}

template <class InheritedAttributeType, class SynthesizedAttributeType>
SynthesizedAttributeType
DistributedMemoryTraversal<InheritedAttributeType, SynthesizedAttributeType>::
getFinalResults()
{
    return finalResults;
}

template <class InheritedAttributeType, class SynthesizedAttributeType>
void
DistributedMemoryTraversal<InheritedAttributeType, SynthesizedAttributeType>::
deleteSerializedAttribute(std::pair<int, void *>) const
{
 // default implementation does nothing
}

// class DistributedMemoryAnalysisPreTraversal

template <class InheritedAttributeType>
DistributedMemoryAnalysisPreTraversal<InheritedAttributeType>::
DistributedMemoryAnalysisPreTraversal(AstTopDownProcessing<InheritedAttributeType> *preTraversal)
    : preTraversal(preTraversal), inFunc(false), nodeCount(0)
{
}

template <class InheritedAttributeType>
InheritedAttributeType
DistributedMemoryAnalysisPreTraversal<InheritedAttributeType>::
evaluateInheritedAttribute(SgNode *node, InheritedAttributeType inheritedValue)
{
 // This is the pre-traversal where the load balancing is computed. To change the
 // grainularity from defining function declarations we have to edit all references
 // to SgFunctionDeclaration (just a few places).  One way to abstract the details
 // of how the level of gainulatity can be adjusted would be to have the predicate
 // below use a functior passed in from the outside.

 // An issue might be that nested defining function declaration (such as those that
 // arise in class declarations) might be a problem for this code.  This should be 
 // tested. A possible solution would be to ignore defining function declaration in 
 // classes (SgMemberFunctionDeclaration).

 // See note from Gergo at the top of the file.

 // The grainularity must match that of the other implementation in the 
 // DistributedMemoryAnalysisPostTraversal.

    // If we are entering a defining function declaration, start counting
    // nodes and save the function declaration node and the inherited value to
    // be passed to it.
    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
    {
        inFunc = true;
        nodeCount = 0;
        funcDecls.push_back(funcDecl);
        initialInheritedValues.push_back(inheritedValue);
    }

    // If we are not inside any function, evaluate the preTraversal.
    // Otherwise, the preTraversal is not to be called, we count the node and
    // return our inheritedValue as a dummy (it is not used anywhere deeper in
    // this subtree).
    if (!inFunc)
    {
        if (preTraversal != NULL)
            return preTraversal->evaluateInheritedAttribute(node, inheritedValue);
        else
            return inheritedValue;
    }
    else
    {
        nodeCount++;
        return inheritedValue;
    }
}

template <class InheritedAttributeType>
void
DistributedMemoryAnalysisPreTraversal<InheritedAttributeType>::
destroyInheritedValue(SgNode *node, InheritedAttributeType inheritedValue)
{
    // If we are outside all functions, the preTraversal computed an
    // inheritedValue before, so give it a chance to destroy it.
    if (!inFunc && preTraversal != NULL)
        preTraversal->destroyInheritedValue(node, inheritedValue);

    // If we are leaving a function, save its number of nodes.
    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
    {
   // A better implementation would address that the funcDecl should be
   // checked against the funcDecls list.  This implementation will 
   // likely fail for the case of nested constructs.  Both for inherited 
   // and synthesised attributes are a likely problem.

        nodeCounts.push_back(nodeCount);
        inFunc = false;
    }
}

template <class InheritedAttributeType>
std::vector<SgFunctionDeclaration *> &
DistributedMemoryAnalysisPreTraversal<InheritedAttributeType>::
get_funcDecls()
{
    return funcDecls;
}

template <class InheritedAttributeType>
std::vector<InheritedAttributeType> &
DistributedMemoryAnalysisPreTraversal<InheritedAttributeType>::
get_initialInheritedValues()
{
    return initialInheritedValues;
}

template <class InheritedAttributeType>
std::vector<size_t> &
DistributedMemoryAnalysisPreTraversal<InheritedAttributeType>::
get_nodeCounts()
{
    return nodeCounts;
}

// class DistributedMemoryAnalysisPostTraversal

template <class SynthesizedAttributeType>
DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType>::
DistributedMemoryAnalysisPostTraversal(AstBottomUpProcessing<SynthesizedAttributeType> *postTraversal,
                                       const std::vector<SynthesizedAttributeType> &functionResults)
  : postTraversal(postTraversal), functionResults(functionResults), functionCounter(0)
{
}

template <class SynthesizedAttributeType>
bool
DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType>::
evaluateInheritedAttribute(SgNode *node, bool inFunction)
{
 // Determine from the inherited attribute and the AST node whether this node is inside a defining function declaration.
    if (inFunction)
        return true;

 // DQ (9/28/2007):
 // This is where the load balancing grainularity is defined and it must match that of the
 // other implementation in the DistributedMemoryAnalysisPreTraversal

    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
        return true;

    return false;
}

template <class SynthesizedAttributeType>
SynthesizedAttributeType
DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType>::
evaluateSynthesizedAttribute(SgNode *node, bool inFunction,
                             DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType>::SynthesizedAttributesList synAttrs)
{
 // If this node is the root of a defining function declaration, return the associated analysis result that was passed
 // to our constructor. If this node is within a defining function declaration, we would like to pretend that we are not
 // even here (since this bottom-up pass is not supposed to traverse functions), so we return some default value. If
 // this node is outside of defining function declarations, perform normal bottom-up evaluation.
    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
        return functionResults[functionCounter++];

    if (inFunction)
    {
        if (!synAttrs.empty())
            return synAttrs.front(); // this is a default attribute computed somewhere below
        else
            return defaultSynthesizedAttribute(inFunction);
    }

    return postTraversal->evaluateSynthesizedAttribute(node, synAttrs);
}

template <class SynthesizedAttributeType>
SynthesizedAttributeType
DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType>::
defaultSynthesizedAttribute(bool)
{
    return postTraversal->defaultSynthesizedAttribute();
}

#if 0
// DQ (9/28/2007): Commented out to avoid use, this is the old interface.

// GB (09/28/2007):
// The DistributedMemoryAnalysis class was the first attempt at doing this. It worked well, but there is no real reason
// to use it, and I do not intend to keep it up to date. If you are a user, please use the DistributedMemoryTraversal
// class instead of this one; if you are a developer who got here because this class fails to compile or whatever, just
// comment it out. (And maybe you could let me know that you did that.)

// class DistributedMemoryAnalysis -- DEPRECATED

template <class InheritedAttributeType>
void
DistributedMemoryAnalysis<InheritedAttributeType>::
performAnalysis(
        SgNode *root,
        InheritedAttributeType rootInheritedAttribute,
        AstTopDownProcessing<InheritedAttributeType> *preTraversal)
{
    /* see what functions to run our analysis on */
    std::pair<int, int> my_limits = computeFunctionIndices(root, rootInheritedAttribute, preTraversal);

    /* run the analysis on the defining function declarations found above */
    for (int i = my_limits.first; i < my_limits.second; i++)
    {
        analyzeSubtree(DistributedMemoryAnalysisBase<InheritedAttributeType>::funcDecls[i],
                       DistributedMemoryAnalysisBase<InheritedAttributeType>::initialInheritedValues[i]);
    }

    /* serialize the analyzer state */
    std::pair<int, void *> state = serialize();

    /* communicate results: first, gather the sizes of the respective states
     * into an array */
    int *stateSizes = new int[DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses()];
    MPI_Allgather(&state.first, 1, MPI_INT,
                  stateSizes, 1, MPI_INT,
                  MPI_COMM_WORLD);

    /* from the state sizes communicated above, compute the total buffer size
     * for all concatenated states, and the indices where each part starts */
    int totalSize = 0;
    int *displacements = new int[DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses()];
    for (int i = 0; i < DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses(); i++)
    {
        displacements[i] = totalSize;
        totalSize += stateSizes[i];
    }

    /* communicate results: gather the actual state information, concatenating
     * it into recvbuf on the root process (the other processes don't get it) */
    unsigned char *recvbuf = new unsigned char[totalSize];
    MPI_Gatherv(state.second, state.first, MPI_UNSIGNED_CHAR,
                recvbuf, stateSizes, displacements, MPI_UNSIGNED_CHAR,
                DistributedMemoryAnalysisBase<InheritedAttributeType>::root_process, MPI_COMM_WORLD);

    /* on the root process, fold up all results by applying
     * addSerializedState() to the states received above */
    if (DistributedMemoryAnalysisBase<InheritedAttributeType>::myID() == DistributedMemoryAnalysisBase<InheritedAttributeType>::root_process)
    {
        for (int i = 0; i < DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses(); i++)
        {
            if (i != DistributedMemoryAnalysisBase<InheritedAttributeType>::root_process)
            {
                /* recvbuf contains the concatenated results from all
                 * processes; the state for process i has size stateSizes[i]
                 * and starts displacements[i] bytes from the beginning of the
                 * buffer */
                std::pair<int, void *> serializedState_i = std::make_pair(stateSizes[i], recvbuf + displacements[i]);
                addSerializedState(serializedState_i);
            }
        }
    }

    /* clean up */
    cleanupSerializedState(state);
    delete[] stateSizes;
    delete[] displacements;
    delete[] recvbuf;
}

template <class InheritedAttributeType>
DistributedMemoryAnalysis<InheritedAttributeType>::
DistributedMemoryAnalysis()
  : DistributedMemoryAnalysisBase<InheritedAttributeType>()
{
}

template <class InheritedAttributeType>
DistributedMemoryAnalysis<InheritedAttributeType>::
~DistributedMemoryAnalysis()
{
}

template <class InheritedAttributeType>
void
DistributedMemoryAnalysis<InheritedAttributeType>::
cleanupSerializedState(std::pair<int, void *>)
{
}
#endif

#endif
