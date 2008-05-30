#include "parallel_compass.h"

using namespace std;
#define DEBUG_OUTPUT true
#define DEBUG_OUTPUT_MORE false




// ************************************************************
// NodeCounter to determine on how to split the nodes
// ************************************************************
class NodeNullDerefCounter: public AstSimpleProcessing
{
public:
  size_t totalNodes;
  size_t totalFunctions;
  size_t *nullDerefNodes;
  SgNode* *nodes;
  bool count;
  size_t interestingNodes;
  NodeNullDerefCounter(bool c) { interestingNodes=0; totalNodes=0; count=true;}
  NodeNullDerefCounter(int numberOfNodes)
    : totalNodes(0), totalFunctions(0), 
      nullDerefNodes(new size_t[numberOfNodes+1]), fileptr(nullDerefNodes) {
    *fileptr = 0;
    interestingNodes=0; 
    count=false;
    nodes = new SgNode*[numberOfNodes+1];
  }
protected:
  // increase the number of nodes and nodes in files
  virtual void visit(SgNode *node) {
    totalNodes++;
    if (!count)
      ROSE_ASSERT(fileptr);
    if (!count) { 
      ++*fileptr;
      //      std::cerr << " fileptr = " << fileptr << "    *fileptr: " << *fileptr << "    totalNodes:  " << totalNodes << endl;
    }
    // must be run postorder!
    if (isSgArrowExp(node) || isSgPointerDerefExp(node)
	|| isSgAssignInitializer(node) || isSgFunctionCallExp(node)) {
      if (!count) {
	fileptr++;
	*fileptr = fileptr[-1];
	nodes[interestingNodes]=node;
	if (DEBUG_OUTPUT_MORE) 
	cerr << " [ " << interestingNodes << " ] adding node " << node->class_name() << endl;
      }
      interestingNodes++;
      //if (!count)
	//std::cout << " ........... fileptr[-1] = " << fileptr[-1] << std::endl;
    }
    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl) {
      totalFunctions++;
    }
  };
  size_t *fileptr;
};



// ************************************************************
// algorithm that splits the files to processors
// ************************************************************
std::pair<int, int> computeNullDerefIndices(SgProject *project, int my_rank, int processes, NodeNullDerefCounter nullderefCounter)
{
  nullderefCounter.traverse(project, postorder);
  int nrOfInterestingNodes = nullderefCounter.interestingNodes;
  // count the amount of nodes in all files
  //cerr << " number of interesting : " << nrOfInterestingNodes << endl;
  //  cerr << " nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] == nullderefCounter.totalNodes  : " <<
  //  nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] << " == " << nullderefCounter.totalNodes  << endl;
  ROSE_ASSERT(nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] == nullderefCounter.totalNodes );

  if (my_rank == 0) {
    std::cout <<  "File: The total amount of nullderefNodes is : " << nrOfInterestingNodes << std::endl;
    std::cout <<  "File: The total amount of functions is : " << nullderefCounter.totalFunctions << std::endl;
    std::cout <<  "File: The total amount of nodes is : " << nullderefCounter.totalNodes << std::endl;
  }

  // split algorithm
  int lo, hi = 0;
  int my_lo, my_hi;
  for (int rank = 0; rank < processes; rank++) {
    const size_t my_nodes_high = (nullderefCounter.totalNodes / processes + 1) * (rank + 1);
    // set lower limit
    lo = hi;
    // find upper limit
    for (hi = lo + 1; hi < nrOfInterestingNodes; hi++) {
      if (nullderefCounter.nullDerefNodes[hi] > my_nodes_high)
	break;
    }

    // make sure all files have been assigned to some process
    if (rank == processes - 1)
      ROSE_ASSERT(hi == nrOfInterestingNodes);

    if (rank == my_rank)  {
      my_lo = lo;
      my_hi = hi;
#if 1
      std::cout << "process " << rank << ": files [" << lo << "," << hi
		<< "[ for a total of "
		<< (lo != 0 ? nullderefCounter.nullDerefNodes[hi-1] - nullderefCounter.nullDerefNodes[lo-1] : nullderefCounter.nullDerefNodes[hi-1])
		<< " nodes" << std::endl;
#endif
      break;
    }
  }

  return std::make_pair(my_lo, my_hi);
}





// ************************************************************
// Use this class to run on function granulty instead of files
// ************************************************************
class MyAnalysis: public DistributedMemoryTraversal<int, std::string>
{
protected:
  std::string analyzeSubtree(SgFunctionDeclaration *funcDecl, int depth)
  {
    std::string funcName = funcDecl->get_name().str();
    std::stringstream s;
    s << "process " << myID() << ": at depth " << depth << ": function " << funcName;
    return s.str();
  }

  // The user must implement this method to pack a synthesized attribute (a string in this case) into an array of bytes
  // for communication. The first component of the pair is the number of bytes in the buffer.
  std::pair<int, void *> serializeAttribute(std::string attribute) const
  {
    int len = attribute.size() + 1;
    char *str = strdup(attribute.c_str());
    return std::make_pair(len, str);
  }

  // This method must be implemented to convert the serialized data to the application's synthesized attribute type.
  std::string deserializeAttribute(std::pair<int, void *> serializedAttribute) const
  {
    return std::string((const char *) serializedAttribute.second);
  }

  // This method is optional (the default implementation is empty). Its job is to free memory that may have been
  // allocated by the serializeAttribute() method.
  void deleteSerializedAttribute(std::pair<int, void *> serializedAttribute) const 
  {
    std::free(serializedAttribute.second);
  }
};




void printPCResults(MyAnalysis& myanalysis, std::vector<CountingOutputObject *> &outputs,
		    unsigned int* output_values,
		    double* times, double* memory,
		    int typeOfPrint
		    ) {
  /* print everything */
  if (my_rank == 0) {

    std::cout << "\n>>>>> results:" << std::endl;
    for (size_t i = 0; i < outputs.size(); i++)
      std::cout << "  " << outputs[i]->name << " " << output_values[i] << std::endl;
    std::cout << std::endl;

    double total_time = 0.0;
    double total_memory = 0.0;
    double min_time = std::numeric_limits<double>::max(), max_time = 0.0;
    int slowest_func=0;
    int fastest_func=0;
    for (size_t i = 0; i < (size_t) processes; i++) {

      std::cout << "processor: " << i << " time: " << times[i] << "  memory: " << memory[i] <<  " MB " << 
	"  real # functions: none." << std::endl;

      total_time += times[i];
      total_memory += memory[i];
      if (min_time > times[i]) {
	min_time = times[i];
	fastest_func = i;
      }
      if (max_time < times[i]) {
	max_time = times[i];
	slowest_func = i;
      }
    }
    std::cout << std::endl;

    std::cout << "\ntotal time: " << total_time << "   total memory : " << total_memory << " MB "
	      << "\n    fastest process: " << min_time << " fastest func: " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[fastest_func]->get_name().str())
	      << "  in File : " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[fastest_func]->get_file_info()->get_filename())
	      << "\n    slowest process: " << max_time << " slowest func: " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[slowest_func]->get_name().str())
	      << "  in File : " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[slowest_func]->get_file_info()->get_filename())
	    << std::endl;
    std::cout << std::endl;
    
    std::cout <<  "The total amount of files is : " << root->numberOfFiles() << std::endl;
    std::cout <<  "The total amount of functions is : " << myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size() << std::endl;
    std::cout <<  "The total amount of nodes is : " << myanalysis.DistributedMemoryAnalysisBase<int>::nrOfNodes << std::endl;
  
  }

}


// ************************************************************
// main function
// ************************************************************
int main(int argc, char **argv)
{

  /* setup MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processes);

  initPCompass(argc, argv);
  ROSE_ASSERT(root);


  /* setup checkers */
  std::vector<AstSimpleProcessing *> traversals;
  std::vector<AstSimpleProcessing *>::iterator t_itr;
  std::vector<Compass::TraversalBase *> bases;
  std::vector<Compass::TraversalBase *>::iterator b_itr;
  std::vector<CountingOutputObject *> outputs;
  std::vector<CountingOutputObject *>::iterator o_itr;

  compassCheckers(traversals, bases, outputs);

  ROSE_ASSERT(traversals.size() == bases.size() && bases.size() == outputs.size());
#if 0
  if (my_rank == 0)
    {
      std::cout << std::endl << "got " << bases.size() << " checkers:";
      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr)
	std::cout << ' ' << (*b_itr)->getName();
      std::cout << std::endl;
    }
#endif


#define CALL_RUN_FOR_CHECKERS 1
#define RUN_COMBINED_CHECKERS 0
#define GERGO_ALGO 0

#define RUN_ON_DEFUSE 0

  /* traverse the files */
  gettime(begin_time);
  double memusage_b = ROSE_MemoryUsage().getMemoryUsageMegabytes();

#if CALL_RUN_FOR_CHECKERS
  if (my_rank==0)
    std::cout << "\n>>> Running in sequence ... " << std::endl;
#endif



#if RUN_ON_DEFUSE
  std::cout << "\n>>> Running on files ... " << std::endl;
  NodeNullDerefCounter nc(true);
  nc.traverse(root, postorder);
  int nrOfInterestingNodes = nc.interestingNodes;
  NodeNullDerefCounter nullderefCounter(nrOfInterestingNodes);

  if (processes==1) {
  /* figure out which files to process on this process */
  std::pair<int, int> bounds = computeNullDerefIndices(root, my_rank, processes, nullderefCounter);
  for (int i = bounds.first; i < bounds.second; i++)
    {
	if (DEBUG_OUTPUT_MORE) 
	  std::cout << "bounds ("<< i<<" [ " << bounds.first << "," << bounds.second << "[ of " << std::endl;
      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
	SgNode* mynode = isSgNode(nullderefCounter.nodes[i]);
	ROSE_ASSERT(mynode);
	if (DEBUG_OUTPUT_MORE) 
	  std::cout << "running checker (" << i << " in ["<< bounds.first << "," << bounds.second 
		    <<"[) : " << (*b_itr)->getName()  << "   on node: " << 
	    mynode->class_name() << std::endl; 
	(*b_itr)->run(mynode);
      }
    }
  } else {
    // apply runtime algorithm to def_use
    cerr << " Dynamic scheduling not implemented yet for single nodes (null deref)" << endl;
  }



#else // run on functions
  FunctionNamesPreTraversal preTraversal;
  MyAnalysis myanalysis;
  int initialDepth=0;

  if (my_rank==0)
    std::cout << "\n>>> Running on functions ... " << std::endl;
#if CALL_RUN_FOR_CHECKERS
#if GERGO_ALGO
  std::pair<int, int> bounds = myanalysis.computeFunctionIndices(root, initialDepth, &preTraversal);
  for (int i = bounds.first; i < bounds.second; i++)
    {
      std::cout << "bounds ("<< i<<" [ " << bounds.first << "," << bounds.second << "[ of " 
		<< (bounds.second-bounds.first) << ")" << "   Nodes: " << myanalysis.myNodeCounts[i] << 
	"   Weight : " << myanalysis.myFuncWeights[i] << std::endl;

      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
	if (DEBUG_OUTPUT_MORE)
	  std::cout << "running checker (" << i << " in ["<< bounds.first << "," << bounds.second 
		    <<"[) : " << (*b_itr)->getName() << " \t on function: " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_name().str()) << 
	    "     in File: " << 	(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_file_info()->get_filename()) << std::endl; 
	(*b_itr)->run(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
      }
    }
#else // my own algo (tps)
  int* dynamicFunctionsPerProcessor = new int[processes];
  for (int k=0;k<processes;k++)
    dynamicFunctionsPerProcessor[k]=0;
  if (DEBUG_OUTPUT_MORE) 
    cout << "Processes = " << processes << endl;
  if (processes==1) {
    std::vector<int> bounds;
    myanalysis.computeFunctionIndicesPerNode(root, bounds, initialDepth, &preTraversal);
    if (DEBUG_OUTPUT_MORE) 
      cout << "bounds size = " << bounds.size() << endl;
    for (int i = 0; i<(int)bounds.size();i++) {
      //if (DEBUG_OUTPUT_MORE) 
      // cout << "bounds [" << i << "] = " << bounds[i] << "   my_rank: " << my_rank << endl;
      if (bounds[i]== my_rank) {
	//if (DEBUG_OUTPUT_MORE) 
	// std::cout << "bounds ("<< i<<"/"<< bounds.size()<<")  - weight: " << (myanalysis.myNodeCounts[i]*
	//								myanalysis.myFuncWeights[i]) << std::endl;
	for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
	  //if (DEBUG_OUTPUT_MORE) 
	    //std::cout << "running checker (" << i << ") : " << (*b_itr)->getName() << " \t on function: " << 
	    //  (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_name().str()) << 
	    //  "     in File: " << 	(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_file_info()->get_filename()) 
	    //      << std::endl; 

	  (*b_itr)->run(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
	}
      }
    }
  } else {
    // apply run-time load balancing
    std::vector<int> bounds;
    myanalysis.computeFunctionIndicesPerNode(root, bounds, initialDepth, &preTraversal);
    // next we need to communicate to 0 that we are ready, if so, 0 sends us the next job
    int currentJob = -1;
    MPI_Status Stat;
    int *res = new int[2];
    res[0]=5;
    res[1]=5;
    bool done = false;
    int jobsDone = 0;
    int scale = 1;
    while (!done) {
      // we are ready, make sure to notify 0
      if (my_rank != 0) {
	//std::cout << " process : " << my_rank << " sending. " << std::endl;
	MPI_Send(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &Stat);
	int min = res[0];
	int max = res[1];
	if (res[0]==-1) 
	  break;

	std::cout << " process : " << my_rank << " receiving nr: [" << min << ":" << max << "[" << std::endl;
	for (int i=min; i<max;i++) {
	  //if (i>=(int)myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size()) {
	  //std::cout << "............ early breakup " << std::endl;
	  //break;
	  //}
	  // ready contains the next number to be processed
	  for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
	    //std::cout << "running checker (" << i << ") : " << (*b_itr)->getName() << " \t on function: " << 
	    // (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_name().str()) << 
	    // "     in File: " << 	(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_file_info()->get_filename()) 
	    //	    << std::endl; 
	    (*b_itr)->run(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
	  }
	}
      }
      if (my_rank == 0) {
	//std::cout << " process : " << my_rank << " receiving. " << std::endl;
	MPI_Recv(res, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &Stat);
	currentJob+=scale;
	if ((currentJob % 20)==19) scale++;
	if (currentJob>=(int)bounds.size()) {
	  res[0] = -1;
	  jobsDone++;
	}      else {
	  res[0] = currentJob;
	  res[1] = currentJob+scale;
	  if (res[1]>=(int)myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size())
	    res[1] = (int)myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size();
	  dynamicFunctionsPerProcessor[Stat.MPI_SOURCE] += scale;
	}
	//      std::cout << " processes done : " << jobsDone << "/" << (processes-1) << std::endl;
	//std::cout << " process : " << my_rank << " sending rank : " << res[0] << std::endl;
	MPI_Send(res, 2, MPI_INT, Stat.MPI_SOURCE, 1, MPI_COMM_WORLD);      
	if (jobsDone==(processes-1))
	  break;
      }
    }
  } // if not processes==1
#endif // own algo
#elif RUN_COMBINED_CHECKERS
  std::cout << "\n>>> Running combined ... " << std::endl;
  AstCombinedSimpleProcessing combined(traversals);
    combined.traverse(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i], preorder);
#else
  int nrOfThreads = 5;
  std::cout << "\n>>> Running shared ... with " << nrOfThreads << " threads per traversal " << std::endl;
  AstSharedMemoryParallelSimpleProcessing parallel(traversals,nrOfThreads);
  for (int i=0; i < (int)myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size(); i++)
    parallel.traverseInParallel(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i], preorder);
#endif // run for checkers


#endif // run on defuse


  gettime(end_time);
  double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  double memusage = memusage_e-memusage_b;
  double my_time = timeDifference(end_time, begin_time);
  std::cout << ">>> Process " << my_rank << " is done. Time: " << my_time << "  Memory: " << memusage << " MB." << std::endl;

  unsigned int *output_values = new unsigned int[outputs.size()];
  double *times = new double[processes];
  double *memory = new double[processes];
  communicateResult(outputs, times, memory, output_values, my_time, memusage);


  printPCResults(myanalysis, outputs, output_values, times, memory, 0);




  /* all done */
  MPI_Finalize();
  delete[] output_values;
  delete[] times;

}


