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


#ifndef DISTRIBUTED_MEMORY_ANALYSIS_IMPLEMENTATION_H
#define DISTRIBUTED_MEMORY_ANALYSIS_IMPLEMENTATION_H

//#include <mpi.h>
#include <math.h>

#define DIS_DEBUG_OUTPUT false
#define RUN_STD true
#define RUN_DECLARATION false
#define ALLGATHER_MPI false

// --------------------------------------------------------------------------
// class DistributedMemoryAnalysisBase -- version by Gergo (based on nodes weight)
// --------------------------------------------------------------------------

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
#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> starting traversal of nodeCounter (preTraversal)" << std::endl;
#endif
    nodeCounter.traverse(root, rootInheritedValue);

    funcDecls = nodeCounter.get_funcDecls();
    initialInheritedValues = nodeCounter.get_initialInheritedValues();
    std::vector<size_t> &nodeCounts = nodeCounter.get_nodeCounts();
    std::vector<size_t> &funcWeights = nodeCounter.get_funcWeights();
    ROSE_ASSERT(funcDecls.size() == initialInheritedValues.size());
    ROSE_ASSERT(funcDecls.size() == nodeCounts.size());
    ROSE_ASSERT(funcDecls.size() == funcWeights.size());

#if DIS_DEBUG_OUTPUT
    std::cout << " >>>  nr of funcs: " << funcDecls.size() << "  inheritedValues:" << initialInheritedValues.size() 
              << "   nodeCounts: " << nodeCounts.size() << std::endl;
    if (funcDecls.size()>0) 
      std::cout << " >>>  example entry 0: funcs[0] " << funcDecls.back() << "  inheritedValues[0]:" 
                << initialInheritedValues.back() << "   nodeCounts[0]: " << nodeCounts.back() << std::endl;
      std::cout << " >>> >>>>>>>>>>>>> done traversal of nodeCounter (preTraversal) \n\n\n" << std::endl;
#endif

    std::vector<size_t>::const_iterator itr;
    size_t totalNodes = 0;
    size_t totalWeight = 0;
    for (itr = nodeCounts.begin(); itr != nodeCounts.end(); ++itr) 
        totalNodes += *itr;
    for (itr = funcWeights.begin(); itr != funcWeights.end(); ++itr) 
        totalWeight += *itr;
    myNodeCounts = nodeCounts;
    myFuncWeights = funcWeights;
    
    if (DistributedMemoryAnalysisBase<InheritedAttributeType>::myID() == 0)
    {
    nrOfNodes = totalNodes;

//#if DIS_DEBUG_OUTPUT
        std::cout << "ROOT - splitting functions: " << funcDecls.size() << ", total nodes: " << totalNodes
                  << "   totalWeight: " << totalWeight << std::endl;
//#endif
    }


    // load balance!!
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
#if DIS_DEBUG_OUTPUT
        std::cout << "SPLITTING - rank : " << rank << " myNodesHigh " << myNodesHigh << "  myNodes: " << myNodes
                  << "  lo: " << lo << "  hi: " << hi << std::endl;
#endif
        // find upper limit
        for (hi = lo + 1; hi < funcDecls.size(); hi++)
        {
#if DIS_DEBUG_OUTPUT
        std::cout << "  SPLITTING - func-hi : " << hi << " myNodes: "  << myNodes << std::endl;
        std::cout << "  SPLITTING - mynodes > mynodesHigh : " << myNodes << " > "  << myNodesHigh << std::endl;
        std::cout << "  SPLITTING - mynodesHigh - myNodes < nodeCounts["<<hi<<"]/2 : " << (myNodesHigh-myNodes)
                  << " < "  << (nodeCounts[hi]/2) << std::endl;
#endif
            if (myNodes > myNodesHigh)
                break;
            else if (myNodesHigh - myNodes < nodeCounts[hi] / 2)
                break;
#if DIS_DEBUG_OUTPUT
	    std::cout << "  hi++" << std::endl;
#endif
            myNodes += nodeCounts[hi];
        }
        nodesSoFar = myNodes;
        functionsPerProcess.push_back(hi - lo);

        // make sure all files have been assigned to some process
        if (rank == processes - 1)
        {
            if (hi != funcDecls.size())
            {
                std::cout << "hi: " << hi << ", funcDecls.size(): " << funcDecls.size() << std::endl;
                std::cout << "It looks like you asked me to analyze too few functions in too many processes."
                    << std::endl << " Please give me fewer processes or a larger program to work on." << std::endl;
            }
            ROSE_ASSERT(hi == funcDecls.size());
        }

        if (rank == my_rank)
        {
            my_lo = lo;
            my_hi = hi;
#if DIS_DEBUG_OUTPUT
            std::cout << "process " << rank << ": functions [" << lo << "," << hi
                << "[ for a total of "
                << myNodes - nodesOfPredecessors
                << " nodes" << std::endl;
#endif
        }
    }

    return std::make_pair(my_lo, my_hi);
}

//compare function for std sort
struct SortDescending : public std::binary_function<std::pair<double, size_t>,std::pair<double, size_t>,bool> 
{
  bool operator()(const std::pair<double, size_t>& s1, const std::pair<double, size_t>& s2) const 
  {
    return (s1.first > s2.first);
  }
};

template <class InheritedAttributeType>
void
DistributedMemoryAnalysisBase<InheritedAttributeType>::
sortFunctions(std::vector<SgFunctionDeclaration*>& funcDecls, std::vector<InheritedAttributeType>& inheritedValues,
              std::vector<size_t>& nodeCounts, std::vector<size_t>& funcWeights)
{

  std::vector<SgFunctionDeclaration*> funcDecls_temp(funcDecls.size());
  std::vector<InheritedAttributeType> inheritedValues_temp(funcDecls.size());
  std::vector<size_t> nodeCounts_temp(funcDecls.size());
  std::vector<size_t> funcWeights_temp(funcDecls.size());

  //  std::cout << "Sorting all functions according to weight..." << std::endl;
  // sort all the vectors
  std::vector<std::pair<double, size_t> > weights(funcDecls.size());
  for (int i=0; i< (int) funcDecls.size(); i++) {
    double currentWeight = (((double)nodeCounts[i]*funcWeights[i])/(double)funcDecls.size()/100);
    weights[i].first = currentWeight;
    weights[i].second = i;
  }
  std::sort(weights.begin(), weights.end(), SortDescending());
  for (int i=0; i< (int) funcDecls.size(); i++) {
    funcDecls_temp[i]=funcDecls[weights[i].second];
    inheritedValues_temp[i]=inheritedValues[weights[i].second];
    nodeCounts_temp[i]=nodeCounts[weights[i].second];
    funcWeights_temp[i]=funcWeights[weights[i].second];
    if (my_rank==0)
	if (DIS_DEBUG_OUTPUT)
      std::cout << "    function : " << funcDecls_temp[i]->get_qualified_name().str()
                << "  weight_mul : " << (weights[i].first) << "   nodes: " << nodeCounts_temp[i]
                << "  weight : " << funcWeights_temp[i] << std::endl;
  }
  funcDecls.swap(funcDecls_temp);
  inheritedValues.swap(inheritedValues_temp);
  nodeCounts.swap(nodeCounts_temp);
  funcWeights.swap(funcWeights_temp);
}



// --------------------------------------------------------------------------
// class DistributedMemoryAnalysisBase -- version by tps (based on computation weight -- dynamic algorithm)
// --------------------------------------------------------------------------
template <class InheritedAttributeType>
void
DistributedMemoryAnalysisBase<InheritedAttributeType>::
computeFunctionIndicesPerNode(
                              SgNode *root, std::vector<int>& functionToProcessor,
        InheritedAttributeType rootInheritedValue,
        AstTopDownProcessing<InheritedAttributeType> *preTraversal)
{
 // This function load balances the work assigned to processors.  Work is currently
 // assigned to processors on the basis SgFunctionDeclaration (if it is a defining declaration).

    DistributedMemoryAnalysisPreTraversal<InheritedAttributeType> nodeCounter(preTraversal);
#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> starting traversal of nodeCounter (preTraversal)" << std::endl;
#endif
    nodeCounter.traverse(root, rootInheritedValue);

    funcDecls = nodeCounter.get_funcDecls();
    initialInheritedValues = nodeCounter.get_initialInheritedValues();
    std::vector<size_t> &nodeCounts = nodeCounter.get_nodeCounts();
    std::vector<size_t> &funcWeights = nodeCounter.get_funcWeights();
    ROSE_ASSERT(funcDecls.size() == initialInheritedValues.size());
    ROSE_ASSERT(funcDecls.size() == nodeCounts.size());
    ROSE_ASSERT(funcDecls.size() == funcWeights.size());

    sortFunctions(funcDecls, initialInheritedValues, nodeCounts, funcWeights);

#if DIS_DEBUG_OUTPUT
    std::cout << " >>>  nr of funcs: " << funcDecls.size() << "  inheritedValues:" << initialInheritedValues.size() 
              << "   nodeCounts: " << nodeCounts.size() << std::endl;
    if (funcDecls.size()>0) 
      std::cout << " >>>  example entry 0: funcs[0] " << funcDecls.back() << "  inheritedValues[0]:" 
                << initialInheritedValues.back() << "   nodeCounts[0]: " << nodeCounts.back() << std::endl;
      std::cout << " >>> >>>>>>>>>>>>> done traversal of nodeCounter (preTraversal) \n\n\n" << std::endl;
#endif

      std::vector<size_t>::const_iterator itr, itr2;
    size_t totalNodes = 0;
    double totalWeight = 0;
    for (itr = nodeCounts.begin(), itr2 = funcWeights.begin(); itr != nodeCounts.end(); ++itr, ++itr2) {
        totalNodes += *itr;
	double currentWeight = (((double)(*itr)*(*itr2))/(double)funcDecls.size()/100);
        totalWeight += currentWeight;
    }
    myNodeCounts = nodeCounts;
    myFuncWeights = funcWeights;
    
    if (DistributedMemoryAnalysisBase<InheritedAttributeType>::myID() == 0)
    {
    nrOfNodes = totalNodes;

//#if DIS_DEBUG_OUTPUT
        std::cout << "ROOT - splitting functions: " << funcDecls.size() << ", total nodes: " << totalNodes
                  << "   totalWeight: " << totalWeight << std::endl;
//#endif
    }

    // changed this so it can be used for defuse for all processors
    //    int start_rank=1;
    int start_rank=0;
    //    std::cerr << " *********** processes : " << processes << std::endl;
    // if only one processor is defined then processor 0 does NOT have to do communication
    // and can hence perform the analysis
    if (processes==1)
      start_rank=0;
    //    std::vector<int> functionToProcessor;
    double processorWeight[processes];
    int nrOfFunctions[processes];
    for (int rank = 0; rank < processes; rank++) {
      processorWeight[rank] = 0;
      nrOfFunctions[rank]=0;
    }
    for (unsigned int i=0; i< funcDecls.size(); i++) {
      double currentWeight = (((double)nodeCounts[i]*funcWeights[i])/(double)funcDecls.size()/100);
      double min =INFINITY;
      int min_rank=start_rank;
      // find the minimum weight processor
      for (int rank = start_rank; rank < processes; rank++) {
        if (processorWeight[rank]<min) {
          min = processorWeight[rank];
          min_rank = rank;
        }
      }
      processorWeight[min_rank]+=currentWeight;
      functionToProcessor.push_back(min_rank);
      nrOfFunctions[min_rank]+=1;
      if (my_rank ==0 ) {
        if (DIS_DEBUG_OUTPUT)
        std::cout << " Function per Processor : " << funcDecls[i]->get_name().str() << "  weight : " <<
          currentWeight << "/" << totalWeight << "  on proc: " << min_rank << std::endl;
      }
    }
    for (int rank = 0; rank < processes; rank++) {
      if (my_rank ==0 ) {
        std::cerr << " Processor : " << rank << "  has " << nrOfFunctions[rank] <<
          " functions. Processor weight: " << processorWeight[rank] << std::endl;
      }
    }

}



// --------------------------------------------------------------------------
// class DistributedMemoryTraversal
// --------------------------------------------------------------------------


template <class InheritedAttributeType, class SynthesizedAttributeType>
void
DistributedMemoryTraversal<InheritedAttributeType, SynthesizedAttributeType>::
performAnalysis(SgNode *root, InheritedAttributeType rootInheritedValue,
                AstTopDownProcessing<InheritedAttributeType> *preTraversal,
                AstBottomUpProcessing<SynthesizedAttributeType> *postTraversal)
{
#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> start computeFunctionIndeces" << std::endl;
#endif
    /* see what functions to run our analysis on */
    std::pair<int, int> my_limits = computeFunctionIndices(root, rootInheritedValue, preTraversal);
#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> done computeFunctionIndeces\n\n\n" << std::endl;
#endif

#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> start serialize results" << std::endl;
#endif
    /* run the analysis on the defining function declarations found above and store the serialized results */
    std::vector<std::pair<int, void *> > serializedResults;
    for (int i = my_limits.first; i < my_limits.second; i++)
    {
        SynthesizedAttributeType result =
            analyzeSubtree(DistributedMemoryAnalysisBase<InheritedAttributeType>::funcDecls[i],
                           DistributedMemoryAnalysisBase<InheritedAttributeType>::initialInheritedValues[i]);
        serializedResults.push_back(serializeAttribute(result));
    }

#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> done serialize results\n" << std::endl;
#endif



#if DIS_DEBUG_OUTPUT
      // in the following we only handle certain functions!!
      std::cout << " >>> >>>>>>>>>>>>> start creating buffer" << std::endl;
#endif
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
#if DIS_DEBUG_OUTPUT
    // buffer contains inherited values, i.e. the nodeCount per function
      std::cout << " total buffer size = " << myTotalSize << "   FunctionsPerProcess: " << myFunctionsPerProcess << std::endl;
#endif
    int sizeSoFar = 0;
    for (int i = 0; i < myFunctionsPerProcess; i++)
    {
        std::memcpy(myBuffer + sizeSoFar, serializedResults[i].second, serializedResults[i].first);
        sizeSoFar += serializedResults[i].first;
     // now that we have made a copy of the serialized attribute, we can free the user-allocated memory
#if DIS_DEBUG_OUTPUT
	std::cout << " buffer ["<<i<<"]  = " << ((char*)serializedResults[i].second)
                  << "   myStateSizes["<<i<<"]  = " << myStateSizes[i] << std::endl;
#endif

        deleteSerializedAttribute(serializedResults[i]);
    }
#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> end creating buffer\n" << std::endl;
#endif



#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> start communication" << std::endl;
#endif
    /* communicate results: first, gather the sizes of the respective states into an array */
      int *stateSizes = NULL;
      int *displacements = NULL;
      if (ALLGATHER_MPI || DistributedMemoryAnalysisBase<InheritedAttributeType>::myID() == 0) {
        displacements = new int[DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses()];
        displacements[0] = 0;
        for (int i = 1; i < DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses(); i++)
          {
            // Displacements are in units of data elements, not bytes
            displacements[i] = displacements[i-1] + DistributedMemoryAnalysisBase<InheritedAttributeType>::functionsPerProcess[i-1];
#if DIS_DEBUG_OUTPUT
            std::cout << " displacements["<<i<<"] = " << displacements[i] << "    == functionsPerProcess[i] " << std::endl;
#endif
          }
        stateSizes = new int[functions];
      }

#if ALLGATHER_MPI
      MPI_Allgatherv(myStateSizes, myFunctionsPerProcess, MPI_INT,
                     stateSizes, &DistributedMemoryAnalysisBase<InheritedAttributeType>::functionsPerProcess[0], 
                     displacements, MPI_INT, MPI_COMM_WORLD);
#else
      MPI_Gatherv(myStateSizes, myFunctionsPerProcess, MPI_INT,
                  stateSizes, &DistributedMemoryAnalysisBase<InheritedAttributeType>::functionsPerProcess[0], 
                  displacements, MPI_INT, 0, MPI_COMM_WORLD);
#endif

#if DIS_DEBUG_OUTPUT
      if (ALLGATHER_MPI || DistributedMemoryAnalysisBase<InheritedAttributeType>::myID() == 0) {
        std::cout << " MPI_Allgatherv: " << functions << std::endl;
        for (unsigned int k=0; k<functions;k++) {
          std::cout << "      stateSize["<<k<<"] = " << stateSizes[k] << std::endl;
        }
      }
#endif

    /* from the state sizes communicated above, compute the total buffer size
     * for all concatenated states, and the indices where each part starts */
    int totalSize = 0;
    int *totalStateSizes = NULL;
    unsigned char* recvbuf = NULL;
    if (ALLGATHER_MPI || DistributedMemoryAnalysisBase<InheritedAttributeType>::myID() == 0) {
      totalStateSizes = new int[DistributedMemoryAnalysisBase<InheritedAttributeType>::numberOfProcesses()];
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
      recvbuf = new unsigned char[totalSize];
    }

    /* communicate results: gather the actual state information, concatenating
     * it into recvbuf on each process */
#if ALLGATHER_MPI
    MPI_Allgatherv(myBuffer, myTotalSize, MPI_UNSIGNED_CHAR,
                   recvbuf, totalStateSizes, displacements, MPI_UNSIGNED_CHAR,
                   MPI_COMM_WORLD);
#else
    MPI_Gatherv(myBuffer, myTotalSize, MPI_UNSIGNED_CHAR,
		recvbuf, totalStateSizes, displacements, MPI_UNSIGNED_CHAR,
		0, MPI_COMM_WORLD);
#endif

#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> end communication\n\n" << std::endl;
#endif


#if DIS_DEBUG_OUTPUT
      std::cout << " >>> >>>>>>>>>>>>> start deserialize" << std::endl;
#endif 
      if (ALLGATHER_MPI || DistributedMemoryAnalysisBase<InheritedAttributeType>::myID() == 0) {
        /* unpack the serialized states and store them away for the post traversal to use */
        functionResults.clear();
        
        int j = 0;
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
                //    std::cout << " getting results " << std::endl;
              }
          }

#if DIS_DEBUG_OUTPUT
	std::cout << " >>> >>>>>>>>>>>>> end deserialize\n" << std::endl;
#endif 
	
#if DIS_DEBUG_OUTPUT
	std::cout << " >>> >>>>>>>>>>>>> start postTraversal" << std::endl;
#endif 

	/* perform the post traversal */
	DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType> postT(postTraversal, functionResults);
	finalResults = postT.traverse(root, false);

#if DIS_DEBUG_OUTPUT
	std::cout << " >>> >>>>>>>>>>>>> end postTraversal\n" << std::endl;
#endif 
	
	/* clean up */
	delete[] stateSizes;
	delete[] totalStateSizes;
	delete[] displacements;
	delete[] recvbuf;
      }
      delete[] myStateSizes;
      delete[] myBuffer;
}





// --------------------------------------------------------------------------
// class DistributedMemoryAnalysisPreTraversal
// --------------------------------------------------------------------------
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
#if RUN_STD
    if (funcDecl) {
      std::string funcname = funcDecl->get_name().str();
  #if DIS_DEBUG_OUTPUT
        std::cout << " >>>>   found function :  " << funcname << std::endl;
  #endif
    }
#else
    if (funcDecl) {
      std::string funcname = funcDecl->get_name().str();
      if (funcname.find("__")!=std::string::npos) {
        stdFunc=true;
      } else {
        stdFunc=false;
  #if DIS_DEBUG_OUTPUT
        std::cout << " >>>>   found function :  " << funcname << std::endl;
  #endif
      }
    }
    if (stdFunc)
	return inheritedValue;
#endif

#if RUN_DECLARATION
    if (funcDecl)
#else
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
#endif
    {
        inFunc = true;
        nodeCount = 0;
	weightNullDeref = 1;
	weightAssignOp = 1;
        funcDecls.push_back(funcDecl);
        initialInheritedValues.push_back(inheritedValue);
	std::string funcname = funcDecl->get_name().str();
#if DIS_DEBUG_OUTPUT
	std::cout << " >>>>   found defining function declaration:  " << funcname
                  << "   inheritedValue: " << inheritedValue << std::endl;
#endif
    }

    // If we are not inside any function, evaluate the preTraversal.
    // Otherwise, the preTraversal is not to be called, we count the node and
    // return our inheritedValue as a dummy (it is not used anywhere deeper in
    // this subtree).
    if (!inFunc)
    {
#if DIS_DEBUG_OUTPUT
      std::cout << "  . outside function : " << node->class_name() << std::endl;
#endif
        if (preTraversal != NULL)
            return preTraversal->evaluateInheritedAttribute(node, inheritedValue);
        else
            return inheritedValue;
    }
    else
    {
#if DIS_DEBUG_OUTPUT
      std::cerr << "     inside function: " << node->class_name() << "  nodeCount =" << nodeCount
                << "   depth=" << inheritedValue << std::endl;
#endif
        nodeCount++;
        // calculate the weight of the function
        // this weight is mostly used for the def-use checker
        if (isSgNode(node))
          weightAssignOp++;
        // for defuse we weight loops heigher
        if (isSgWhileStmt(node) || isSgForStatement(node)
            || isSgDoWhileStmt(node)) {
            weightAssignOp+=(weightAssignOp)/5;
        }
        // the following weight should be used if null-deref is checked for
        /*
        if (isSgArrowExp(node) || isSgPointerDerefExp(node) ||
            isSgAssignInitializer(node) || isSgFunctionCallExp(node) ) { 
          weightNullDeref++;
          } else if (isSgAssignOp(node)) {
          weightAssignOp++;
        }
        */
        return inheritedValue;
    }
}

template <class InheritedAttributeType>
void
DistributedMemoryAnalysisPreTraversal<InheritedAttributeType>::
destroyInheritedValue(SgNode *node, InheritedAttributeType inheritedValue)
{
#if RUN_STD
#else
  if (stdFunc)
        return ;
#endif

    // If we are outside all functions, the preTraversal computed an
    // inheritedValue before, so give it a chance to destroy it.
    if (!inFunc && preTraversal != NULL)
      preTraversal->destroyInheritedValue(node, inheritedValue);

    // If we are leaving a function, save its number of nodes.
    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);
#if RUN_DECLARATION
    if (funcDecl)
#else
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
#endif
    {
   // A better implementation would address that the funcDecl should be
   // checked against the funcDecls list.  This implementation will 
   // likely fail for the case of nested constructs.  Both for inherited 
   // and synthesised attributes are a likely problem.

        nodeCounts.push_back(nodeCount);
	//	double result = (double)weightAssignOp*(double)1/log((double)weightNullDeref+1);
	//	funcWeights.push_back((int)result);
	//	funcWeights.push_back(weightAssignOp*weightNullDeref);
	funcWeights.push_back(weightAssignOp);
	//	std::cout << " pushing back func : " << funcDecl->get_name().str() << "   weightAssignOp : " << weightAssignOp << "  weightNullDeref : " << weightNullDeref <<
	//  "  1/log(weightNullDeref) : " << (1/(log(weightNullDeref))) << "   result = " << result << std::endl;
        inFunc = false;
#if DIS_DEBUG_OUTPUT
	std::cout << " destroying - save nodes  " << nodeCount << std::endl;
#endif
    }
}





// --------------------------------------------------------------------------
// class DistributedMemoryAnalysisPostTraversal
// --------------------------------------------------------------------------


//bool
//DistributedMemoryAnalysisNamespace::postTraversalEvaluateInheritedAttribute(SgNode *node, bool inFunction)
template <class SynthesizedAttributeType>
bool
DistributedMemoryAnalysisPostTraversal<SynthesizedAttributeType>::
evaluateInheritedAttribute(SgNode *node, bool inFunction)
{
    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);

#if RUN_STD
#else
    std::string funcname="";
    if (funcDecl) {
      funcname = funcDecl->get_name().str();
      if (funcname.find("__")!=std::string::npos) 
        stdFunc=true;
      else
        stdFunc=false;
    }  
    if (stdFunc)
        return false;
  #if DIS_DEBUG_OUTPUT
    if (funcDecl)
      std::cout << " >>> postTraversal - inheritedAttribute: found function:  " << funcname << std::endl;
  #endif
#endif
  #if DIS_DEBUG_OUTPUT
    std::cout << " postTraversal - inheritedAttribute on node:  " << node->class_name() << 
      "   parentEval (inFunction?): " << inFunction << std::endl;
  #endif

 // Determine from the inherited attribute and the AST node whether this node is inside a defining function declaration.
    if (inFunction)
        return true;

 // DQ (9/28/2007):
 // This is where the load balancing grainularity is defined and it must match that of the
 // other implementation in the DistributedMemoryAnalysisPreTraversal

#if RUN_DECLARATION
    if (funcDecl)
#else
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
#endif
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
#if RUN_STD
#else
    std::string funcname="none";
    if (funcDecl) {
      funcname = funcDecl->get_name().str();
      if (funcname.find("__")!=std::string::npos) {
        stdFunc=true;
      } else {
        stdFunc=false;
  #if DIS_DEBUG_OUTPUT
      std::cout << " .. post synthesizedAttribute function: " << funcname << "   id-nr:" << functionCounter << std::endl;
  #endif
      }
    }
    if (stdFunc) {
      return defaultSynthesizedAttribute(inFunction);
    } 
#endif

#if DIS_DEBUG_OUTPUT
    std::cout << "   post in (NODE): >>    " << node->class_name() << "  currentNode (inFunction?) " << inFunction << std::endl;
#endif

#if RUN_DECLARATION
    if (funcDecl)
#else
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
#endif
      {
        return functionResults[functionCounter++];
    }

    if (inFunction)
    {
      if (!synAttrs.empty()) {
        SynthesizedAttributeType attr = synAttrs.front(); // this is a default attribute computed somewhere below
        return attr;
      } else {
            return defaultSynthesizedAttribute(inFunction);
      }
    }

#if DIS_DEBUG_OUTPUT
        std::cout << " ... EVALUATE synthesized Attribute "  << std::endl;
#endif
    return postTraversal->evaluateSynthesizedAttribute(node, synAttrs);
}

#endif


