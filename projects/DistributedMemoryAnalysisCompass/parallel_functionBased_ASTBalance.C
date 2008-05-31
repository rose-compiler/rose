#include "parallel_compass.h"

#include "functionLevelTraversal.h"


using namespace std;
#define DEBUG_OUTPUT true
#define DEBUG_OUTPUT_MORE true




void printPCResults(MyAnalysis& myanalysis, std::vector<CountingOutputObject *> &outputs,
		    unsigned int* output_values,
		    double* times, double* memory
		    ) {
  if (processes > 1 ) {
    cerr << " Processes specified: " << processes << " -- Currently the combined and shared memory model does not run in distributed mode!" << endl;
    exit(0);
  }
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

    std::cout << "\ntotal time: " << total_time << "   total memory : " << total_memory << " MB " << endl;
    std::cout << "\nfastest func : " << fastest_func << "  slowest_func : " << slowest_func ;
    std::cout      << "\n    fastest process: " << min_time << " fastest   in file: " << root->get_file(fastest_func).getFileName() 
		     << "\n    slowest process: " << max_time << " slowest   in file: " << root->get_file(slowest_func).getFileName()
		     << std::endl;
    std::cout << std::endl;

    if (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size() ==0) {
      cerr << " ERROR: nr of functions : " << myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size() << endl;
    } else {
      SgFunctionDeclaration* func_fast = myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[fastest_func];
      ROSE_ASSERT(func_fast);
      std::cout << "\ntotal time: " << total_time << "   total memory : " << total_memory << " MB "
		<< "\n    fastest process: " << min_time << " fastest func: " << (func_fast->get_name().str());
      std::cout << "  in File : " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[fastest_func]->get_file_info()->get_filename())
		<< "\n    slowest process: " << max_time << " slowest func: " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[slowest_func]->get_name().str());
      std::cout << "  in File : " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[slowest_func]->get_file_info()->get_filename())
		<< std::endl;
      
      std::cout << std::endl;
    }    
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


  FunctionNamesPreTraversal preTraversal;
  MyAnalysis myanalysis;
  int initialDepth=0;

  if (my_rank==0)
    std::cout << "\n>>> Running on functions ... " << std::endl;
    std::pair<int, int> bounds = myanalysis.computeFunctionIndices(root, initialDepth, &preTraversal);
  if (sequential) {
    if (my_rank==0)
      std::cout << "\n>>> Running in sequence ... " << std::endl;

    // gergos original algorithm
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
  } else if (combined) {
    std::cout << "\n>>> Running combined ...    funcDecls size : " << 
      myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size() << std::endl;
    AstCombinedSimpleProcessing combined(traversals);
    for (int i = bounds.first; i < bounds.second; i++)
      combined.traverse(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i], preorder);
  } else {
    std::cout << "\n>>> Running shared ... with " << nrOfThreads << " threads per traversal -- funcDecls size : " << 
      myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size() << std::endl;
    AstSharedMemoryParallelSimpleProcessing parallel(traversals,nrOfThreads);
    for (int i = bounds.first; i < bounds.second; i++)
      parallel.traverseInParallel(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i], preorder);
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


  printPCResults(myanalysis, outputs, output_values, times, memory);


  /* all done */
  MPI_Finalize();
  delete[] output_values;
  delete[] times;

}


