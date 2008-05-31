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




void printPCResults(std::vector<CountingOutputObject *> &outputs,
		    unsigned int* output_values,
		    double* times, double* memory
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
	      << "\n    fastest process: " << min_time << " fastest   in file: " << root->get_file(fastest_func).getFileName() 
	      << "\n    slowest process: " << max_time << " slowest   in file: " << root->get_file(slowest_func).getFileName()
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
  if (my_rank==0)
    std::cout << "\n>>> Running in sequence ... " << std::endl;



  std::cout << "\n>>> Running defuse ... " << std::endl;
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



  gettime(end_time);
  double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  double memusage = memusage_e-memusage_b;
  double my_time = timeDifference(end_time, begin_time);
  std::cout << ">>> Process " << my_rank << " is done. Time: " << my_time << "  Memory: " << memusage << " MB." << std::endl;

  unsigned int *output_values = new unsigned int[outputs.size()];
  double *times = new double[processes];
  double *memory = new double[processes];
  communicateResult(outputs, times, memory, output_values, my_time, memusage);


  printPCResults(outputs, output_values, times, memory);


  /* all done */
  MPI_Finalize();
  delete[] output_values;
  delete[] times;

}


