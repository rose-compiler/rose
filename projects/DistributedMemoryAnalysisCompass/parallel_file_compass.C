#include "parallel_compass.h"



using namespace std;
#define DEBUG_OUTPUT true
#define DEBUG_OUTPUT_MORE false

using namespace Compass;


// ************************************************************
// NodeFileCounter to determine on how to split the nodes
// ************************************************************
class NodeFileCounter: public AstSimpleProcessing
{
public:
  size_t totalNodes;
  size_t totalFunctions;
  size_t *fileNodes;
  NodeFileCounter(int numberOfFiles)
    : totalNodes(0), totalFunctions(0), 
      fileNodes(new size_t[numberOfFiles+1]), fileptr(fileNodes) {
    *fileptr = 0;
  }
protected:
  // increase the number of nodes and nodes in files
  virtual void visit(SgNode *node) {
    totalNodes++;
    ++*fileptr;
    //std::cerr << " fileptr = " << fileptr << "  : " << *fileptr << endl;
    // must be run postorder!
    if (isSgFile(node)) {
      fileptr++;
      *fileptr = fileptr[-1];
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
std::pair<int, int> computeFileIndices(SgProject *project, int my_rank, int processes)
{
  // count the amount of nodes in all files
  NodeFileCounter nc(project->numberOfFiles());
  nc.traverse(project, postorder);
  ROSE_ASSERT(nc.fileNodes[project->numberOfFiles() - 1] == nc.totalNodes - 1);

  if (my_rank == 0) {
    std::cout <<  "File: The total amount of files is : " << project->numberOfFiles() << std::endl;
    std::cout <<  "File: The total amount of functions is : " << nc.totalFunctions << std::endl;
    std::cout <<  "File: The total amount of nodes is : " << nc.totalNodes << std::endl;
  }

  // split algorithm
  int lo, hi = 0;
  int my_lo, my_hi;
  for (int rank = 0; rank < processes; rank++) {
    const size_t my_nodes_high = (nc.totalNodes / processes + 1) * (rank + 1);
    // set lower limit
    lo = hi;
    // find upper limit
    for (hi = lo + 1; hi < project->numberOfFiles(); hi++) {
      if (nc.fileNodes[hi] > my_nodes_high)
	break;
    }

    // make sure all files have been assigned to some process
    if (rank == processes - 1)
      ROSE_ASSERT(hi == project->numberOfFiles());

    if (rank == my_rank)  {
      my_lo = lo;
      my_hi = hi;
#if 0
      std::cout << "process " << rank << ": files [" << lo << "," << hi
		<< "[ for a total of "
		<< (lo != 0 ? nc.fileNodes[hi-1] - nc.fileNodes[lo-1] : nc.fileNodes[hi-1])
		<< " nodes" << std::endl;
#endif
      break;
    }
  }

  return std::make_pair(my_lo, my_hi);
}





void printPCResults(CountingOutputObject  &outputs,
		    unsigned int* output_values,
		    double* times, double* memory,
		    int typeOfPrint
		    ) {
  /* print everything */
  if (my_rank == 0) {

    std::cout << "\n>>>>> results:" << std::endl;
    std::map<std::string, unsigned int> ::iterator o_itr;
    for (o_itr = outputs.counts.begin(); o_itr != outputs.counts.end(); ++o_itr) 
      std::cout << "  " << o_itr->first << " " << o_itr->second << std::endl;
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
    std::cout <<  "The total amount of files is : " << root->numberOfFiles() << std::endl;

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

  initPCompass(argc, argv, processes);

  ROSE_ASSERT(root);

  /* setup checkers */
  std::vector<AstSimpleProcessingWithRunFunction *> traversals;
  std::vector<AstSimpleProcessingWithRunFunction *>::iterator t_itr;
  std::vector<const Compass::Checker *> bases;
  std::vector<const Compass::Checker *> basesAll;
  std::vector<const Compass::Checker *>::iterator b_itr;
  CountingOutputObject  outputs ;

  //  compassCheckers(traversals, bases, outputs);
  Compass::Parameters params(Compass::findParameterFile());
  buildCheckers(basesAll, params, outputs, root);

  for (b_itr = basesAll.begin(); b_itr != basesAll.end(); ++b_itr) {
    const Compass::CheckerUsingAstSimpleProcessing* astChecker = 
      dynamic_cast<const Compass::CheckerUsingAstSimpleProcessing*>(*b_itr);
    if (astChecker!=NULL) {
      bases.push_back(astChecker);
      traversals.push_back(astChecker->createSimpleTraversal(params, &outputs));
    }
  }
  outputs.fillOutputList(bases);

  //  ROSE_ASSERT(traversals.size() == bases.size() && bases.size() == outputs.size());
  if (DEBUG_OUTPUT_MORE) 
    if (my_rank == 0)
      {
	std::cout << std::endl << "got " << bases.size() << " checkers:";
	for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr)
	  std::cout << ' ' << (*b_itr)->checkerName;
	std::cout << std::endl;
      }

  for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr)
    Compass::runPrereqs(*b_itr, root);

 /* traverse the files */
  Compass::gettime(begin_time);
  double memusage_b = ROSE_MemoryUsage().getMemoryUsageMegabytes();

  if (sequential)
    if (my_rank==0)
      std::cout << "\n>>> Running in sequence ... " << std::endl;



  std::cout << "\n>>> Running on files ... " << std::endl;
  /* figure out which files to process on this process */
  std::pair<int, int> bounds = computeFileIndices(root, my_rank, processes);
  for (int i = bounds.first; i < bounds.second; i++)
    {
      std::cout << "bounds ("<< i<<" [ " << bounds.first << "," << bounds.second << "[ of " << std::endl;

      if (sequential) {
	for (b_itr = bases.begin(), t_itr = traversals.begin(); b_itr != bases.end(); ++b_itr, ++t_itr) {
	  if (DEBUG_OUTPUT_MORE) 
	    std::cout << "running checker (" << i << " in ["<< bounds.first << "," << bounds.second 
		      <<"[) : " << (*b_itr)->checkerName << " \t on " << (root->get_file(i).getFileName()) << std::endl; 
	  (*t_itr)->run(&root->get_file(i));
	}
      } else if (combined) {
	std::cout << "\n>>> Running combined ... " << std::endl;
	//AstCombinedSimpleProcessing combined(traversals);
	//combined.traverse(&root->get_file(i), preorder);
      } else {
	std::cout << "\n>>> Running shared ... with " << nrOfThreads << " threads per traversal " << std::endl;
	//AstSharedMemoryParallelSimpleProcessing parallel(traversals,nrOfThreads);
	//parallel.traverseInParallel(&root->get_file(i), preorder);
      }
    }


  Compass::gettime(end_time);
  double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  double memusage = memusage_e-memusage_b;
  double my_time = Compass::timeDifference(end_time, begin_time);
  std::cout << ">>> Process " << my_rank << " is done. Time: " << my_time << "  Memory: " << memusage << " MB." << std::endl;

  unsigned int *output_values = new unsigned int[outputs.counts.size()];
  double *times = new double[processes];
  double *memory = new double[processes];
  communicateResult(outputs, times, memory, output_values, my_time, memusage);

  printPCResults(outputs, output_values, times, memory, 1);

  /* all done */
  MPI_Finalize();
  delete[] output_values;
  delete[] times;

}


