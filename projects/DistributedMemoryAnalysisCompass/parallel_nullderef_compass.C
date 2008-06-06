#include "parallel_compass.h"


using namespace std;
#define DEBUG_OUTPUT true
#define DEBUG_OUTPUT_MORE true


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
    if (!count) {
      ROSE_ASSERT(fileptr);
      ++*fileptr;
    }
    // must be run postorder!
    // which nodes are interesting? we need to perform the weighting on this.
    //    if (isSgArrowExp(node) || isSgPointerDerefExp(node)
    //	|| isSgAssignInitializer(node) || isSgFunctionCallExp(node)) {
      if (!count) {
	fileptr++;
	*fileptr = fileptr[-1];
	nodes[interestingNodes]=node;
	//if (DEBUG_OUTPUT_MORE ) 
	//cerr << " [ " << interestingNodes << " ] adding node " << node->class_name() << endl;
      }
      interestingNodes++;
      //}
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
    std::cout <<  "0: File: The total amount of interesting nodes is : " << nrOfInterestingNodes << std::endl;
    std::cout <<  "0: File: The total amount of functions is : " << nullderefCounter.totalFunctions << std::endl;
    std::cout <<  "0: File: The total amount of nodes is : " << nullderefCounter.totalNodes << std::endl;
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
// algorithm that splits the files to processors
// ************************************************************
void computeIndicesPerNode(SgProject *project, std::vector<int>& nodeToProcessor,
					  int my_rank, int processes, NodeNullDerefCounter nullderefCounter)
{
  nullderefCounter.traverse(project, postorder);
  int nrOfInterestingNodes = nullderefCounter.interestingNodes;
  // count the amount of nodes in all files
  //cerr << " number of interesting : " << nrOfInterestingNodes << endl;
  //  cerr << " nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] == nullderefCounter.totalNodes  : " <<
  //  nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] << " == " << nullderefCounter.totalNodes  << endl;
  ROSE_ASSERT(nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] == nullderefCounter.totalNodes );

  if (my_rank == 0) {
    std::cout <<  "0: File: The total amount of interesting nodes is : " << nrOfInterestingNodes << std::endl;
    std::cout <<  "0: File: The total amount of functions is : " << nullderefCounter.totalFunctions << std::endl;
    std::cout <<  "0: File: The total amount of nodes is : " << nullderefCounter.totalNodes << std::endl;
  }

  std::vector<SgNode*> nodeDecls(nullderefCounter.totalNodes);
  std::vector<double> nodeWeights(nullderefCounter.totalNodes);

  std::vector<std::pair<double, size_t> > weights(nullderefCounter.totalNodes);
    for (unsigned int i=0; i< nullderefCounter.totalNodes; i++) {
      SgNode* node = nullderefCounter.nodes[i];
      double currentWeight = 0.0001;
      if (isSgArrowExp(node) || isSgPointerDerefExp(node)
    	|| isSgAssignInitializer(node) || isSgFunctionCallExp(node)) {
	//SgNode* parent = node->get_parent();
	//while (!isSgFunctionDefinition(parent) && !isSgProject(parent))
	//  parent=parent->get_parent();
	//NodeNullDerefCounter nrNiF = NodeNullDerefCounter(true);
	//nrNiF.traverse(node, postorder);
	currentWeight = 0.05;//*nrNiF.totalNodes;
      } else if (isSgFunctionDefinition(node)) {
	NodeNullDerefCounter nrNiF = NodeNullDerefCounter(true);
	nrNiF.traverse(node, postorder);
	currentWeight = 0.001*nrNiF.totalNodes;
      }
      weights[i].first = currentWeight;
      weights[i].second = i;
    }
    std::sort(weights.begin(), weights.end(), SortDescending());
    for (int i=0; i< (int) nullderefCounter.totalNodes; i++) {
      nodeDecls[i]=nullderefCounter.nodes[weights[i].second];
      nodeWeights[i]=weights[i].first;
      //      if (my_rank==0)
      //	  std::cout << "    node : " << nodeDecls[i]->class_name() << 
      //    "  weight : " << nodeWeights[i] << std::endl;
    }


    int start_rank=1;
    if (processes==1)
      start_rank=0;

    double processorWeight[processes];
    int nrOfNodes[processes];
    for (int rank = 0; rank < processes; rank++) {
      processorWeight[rank] = 0;
      nrOfNodes[rank]=0;
    }

    for (unsigned int i=0; i< nullderefCounter.totalNodes; i++) {
      double currentWeight = nodeWeights[i];
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
      nodeToProcessor.push_back(min_rank);
      nrOfNodes[min_rank]+=1;
      //      if (my_rank ==0 ) {
      //std::cout << " Nodes per Processor : " << node->class_name() << "  weight : " <<
      //  currentWeight /*<< "/" << totalWeight */<< "  on proc: " << min_rank << std::endl;
      //}
    }

    if (my_rank ==0 ) {
      cout << " " << endl;
      for (int rank = 0; rank < processes; rank++) {
	std::cout << "Processor : " << rank << "  has " << nrOfNodes[rank] <<
	  " nodes. Processor weight: " << processorWeight[rank] << std::endl;
      }
    }

}



void printPCResults(std::vector<CountingOutputObject *> &outputs,
		    unsigned int* output_values,
		    double* times, double* memory//, double* nr_func
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
	"  real # functions:  " << (dynamicFunctionsPerProcessor[i]) << std::endl;//   static func: " << nr_func[i] << std::endl;

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
      	      << "\n    fastest process: " << min_time // << " fastest   in file: " << root->get_file(fastest_func).getFileName() 
      	      << "\n    slowest process: " << max_time //<< " slowest   in file: " << root->get_file(slowest_func).getFileName()
	      << std::endl;
    std::cout << std::endl;
  
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
  if (DEBUG_OUTPUT_MORE) 
  if (my_rank == 0)
    {
      std::cout << std::endl << "got " << bases.size() << " checkers:";
      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr)
	std::cout << ' ' << (*b_itr)->getName();
      std::cout << std::endl;
    }



  /* traverse the files */
  gettime(begin_time);
  double memusage_b = ROSE_MemoryUsage().getMemoryUsageMegabytes();

  if (sequential)
    if (my_rank==0) {
    std::cout << "\n>>> Running in sequence ... " << std::endl;
    std::cout << "\n>>> Running defuse ... " << std::endl;
    }

  NodeNullDerefCounter nc(true);
  nc.traverse(root, postorder);
  int totalnr = nc.totalNodes;
  int nrOfInterestingNodes = nc.interestingNodes;
  NodeNullDerefCounter nullderefCounter(nrOfInterestingNodes);

  dynamicFunctionsPerProcessor = new int[processes];
  for (int k=0;k<processes;k++)
    dynamicFunctionsPerProcessor[k]=0;

  //  std::pair<int, int> bounds;
  std::vector<int> bounds;
  if (processes==1) {
  /* figure out which files to process on this process */
    computeIndicesPerNode(root, bounds, my_rank, processes, nullderefCounter);
    //   bounds = computeNullDerefIndices(root, my_rank, processes, nullderefCounter);
      if (DEBUG_OUTPUT_MORE) 
	cout << "bounds size = " << bounds.size() << endl;
      for (int i = 0; i<(int)bounds.size();i++) {
	if (DEBUG_OUTPUT_MORE) 
	 cout << "bounds [" << i << "] = " << bounds[i] << "   my_rank: " << my_rank << endl;
	//  for (int i = bounds.first; i < bounds.second; i++)
	//{
	//if (DEBUG_OUTPUT_MORE) 
	// std::cout << my_rank << ": bounds ("<< i<<" [ " << bounds.first << "," << bounds.second << "[ of " << nrOfInterestingNodes << std::endl;
	if (bounds[i]== my_rank) 
      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
	SgNode* mynode = isSgNode(nullderefCounter.nodes[i]);
	ROSE_ASSERT(mynode);
	//if (DEBUG_OUTPUT_MORE) 
	// std::cout << "running checker (" << i << " in ["<< bounds.first << "," << bounds.second 
	//	    <<"[) : " << (*b_itr)->getName()  << "   on node: " << 
	//  mynode->class_name() << std::endl; 
	(*b_itr)->visit(mynode);
      }
    }
  } else {
    // apply runtime algorithm to def_use
    cerr << " Dynamic scheduling ..." << endl;

      // apply run-time load balancing
      //      std::vector<int> bounds;
      computeIndicesPerNode(root, bounds, my_rank, processes, nullderefCounter);
      // next we need to communicate to 0 that we are ready, if so, 0 sends us the next job
      int currentJob = -1;
      MPI_Status Stat;
      int *res = new int[2];
      res[0]=5;
      res[1]=5;
      bool done = false;
      int jobsDone = 0;
      int scale = 250;
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

	  std::cout << " process : " << my_rank << " receiving nr: [" << min << ":" << max << "[  of " << 
	    totalnr << "      range : " << (max-min) << std::endl;
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
	      SgNode* mynode = isSgNode(nullderefCounter.nodes[i]);
	      ROSE_ASSERT(mynode);
	      (*b_itr)->visit(mynode);
	      //(*b_itr)->run(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
	    }
	  }
	}
	if (my_rank == 0) {
	  //std::cout << " process : " << my_rank << " receiving. " << std::endl;
	  MPI_Recv(res, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &Stat);
	  currentJob+=scale;
	  if ((currentJob % 50)==49) scale+=5;
	  if (currentJob>=(int)bounds.size()) {
	    res[0] = -1;
	    jobsDone++;
	  }      else {
	    res[0] = currentJob;
	    res[1] = currentJob+scale;
	    //	    if (res[1]>=(int)myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size())
	    // res[1] = (int)myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size();
	    if (res[1]>=(int)bounds.size())//nullderefCounter.totalNodes)
	      res[1] = bounds.size();//nullderefCounter.totalNodes;
	    //cerr << " sending res[1] : " << res[1] << endl;
	    dynamicFunctionsPerProcessor[Stat.MPI_SOURCE] += scale;
	  }
	  //      std::cout << " processes done : " << jobsDone << "/" << (processes-1) << std::endl;
	  //std::cout << " process : " << my_rank << " sending rank : " << res[0] << std::endl;
	  MPI_Send(res, 2, MPI_INT, Stat.MPI_SOURCE, 1, MPI_COMM_WORLD);      
	  if (jobsDone==(processes-1))
	    break;
	}
      }
  }




  gettime(end_time);
  double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  double memusage = memusage_e-memusage_b;
  double my_time = timeDifference(end_time, begin_time);
  std::cout << ">>> Process " << my_rank << " is done. Time: " << my_time << "  Memory: " << memusage << " MB." << std::endl;

  unsigned int *output_values = new unsigned int[outputs.size()];
  double *times = new double[processes];
  double *memory = new double[processes];
  //double *nr_func = new double[processes];
  //double thisfunction = bounds.second-bounds.first;
  communicateResult(outputs, times, memory, output_values, my_time, memusage);//, nr_func, thisfunction);


  printPCResults(outputs, output_values, times, memory);//, nr_func);


  /* all done */
  MPI_Finalize();
  delete[] output_values;
  delete[] times;

}


