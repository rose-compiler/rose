#include <mpi.h>
#include "rose.h"
#include "parallel_compass.h"

#include "functionLevelTraversal.h"


using namespace std;
#define DEBUG_OUTPUT true
#define DEBUG_OUTPUT_MORE false

using namespace Compass;


void printPCResults(MyAnalysis& myanalysis, CountingOutputObject &outputs,
		    unsigned int* output_values,
		    double* times, double* memory
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
	"  real # functions: " <<(dynamicFunctionsPerProcessor[i])  << std::endl;


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

    if (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size() ==0) {
      cerr << " ERROR: nr of functions : " << myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size() << endl;
    } else {
      std::cout << "\ntotal time: " << total_time << "   total memory : " << total_memory << " MB "
		<< "\n    fastest process: " << min_time << " fastest func: " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[fastest_func]->get_name().str())
		<< "  in File : " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[fastest_func]->get_file_info()->get_filename())
		<< "\n    slowest process: " << max_time << " slowest func: " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[slowest_func]->get_name().str())
		<< "  in File : " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[slowest_func]->get_file_info()->get_filename())
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

  MPI_Barrier(MPI_COMM_WORLD);
  FunctionNamesPreTraversal preTraversal;
  MyAnalysis myanalysis;
  int initialDepth=0;

  if (my_rank==0)
    std::cout << "\n>>> Running on functions ... " << std::endl;
  std::vector<int> bounds;
  if (sequential) {
    dynamicFunctionsPerProcessor = new int[processes];
    for (int k=0;k<processes;k++)
      dynamicFunctionsPerProcessor[k]=0;
    if (DEBUG_OUTPUT_MORE) 
      cout << "Processes = " << processes << endl;
    if (processes==1) {

      myanalysis.computeFunctionIndicesPerNode(root, bounds, initialDepth, &preTraversal);
      if (DEBUG_OUTPUT_MORE) 
	cout << "bounds size = " << bounds.size() << endl;
      for (int i = 0; i<(int)bounds.size();i++) {
	if (DEBUG_OUTPUT_MORE) 
	  cout << "bounds [" << i << "] = " << bounds[i] << "   my_rank: " << my_rank << endl;
	if (bounds[i]== my_rank) {
	  //if (DEBUG_OUTPUT_MORE) 
	  // std::cout << "bounds ("<< i<<"/"<< bounds.size()<<")  - weight: " << (myanalysis.myNodeCounts[i]*
	  //								myanalysis.myFuncWeights[i]) << std::endl;
	  for (t_itr = traversals.begin(); t_itr != traversals.end(); ++t_itr) {
	    //if (DEBUG_OUTPUT_MORE) 
	    //std::cout << "running checker (" << i << ") : " << (*b_itr)->getName() << " \t on function: " << 
	    //  (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_name().str()) << 
	    //  "     in File: " << 	(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_file_info()->get_filename()) 
	    //      << std::endl; 
	    (*t_itr)->run(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
	  }
	}
      }
    } else {
      // apply run-time load balancing
      //      std::vector<int> bounds;
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
	    for (t_itr = traversals.begin(); t_itr != traversals.end(); ++t_itr) {
	      //std::cout << "running checker (" << i << ") : " << (*b_itr)->getName() << " \t on function: " << 
	      // (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_name().str()) << 
	      // "     in File: " << 	(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_file_info()->get_filename()) 
	      //	    << std::endl; 
	      (*t_itr)->run(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
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

  } else if (combined) {
    if (processes > 1 ) {
      cerr << "  Processes specified: " << processes << " -- Currently the combined and shared memory model does not run in distributed mode!" << endl;
      exit(0);
    }
    std::cout << "\n>>> Running combined ... " << std::endl;
    //AstCombinedSimpleProcessing combined(traversals);
    //for (int i=0; i < (int)myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size(); i++)
    //  combined.traverse(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i], preorder);
  } else {
    if (processes > 1 ) {
      cerr << "  Processes specified: " << processes << " -- Currently the combined and shared memory model does not run in distributed mode!" << endl;
      exit(0);
    }
    std::cout << "\n>>> Running shared ... with " << nrOfThreads << " threads per traversal " << std::endl;
    //AstSharedMemoryParallelSimpleProcessing parallel(traversals,nrOfThreads);
    //for (int i=0; i < (int)myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size(); i++)
    //  parallel.traverseInParallel(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i], preorder);
  }



  Compass::gettime(end_time);
  double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  double memusage = memusage_e-memusage_b;
  double my_time = Compass::timeDifference(end_time, begin_time);
  std::cout << ">>> Process " << my_rank << " is done. Time: " << my_time << "  Memory: " << memusage << " MB." << std::endl;

  unsigned int *output_values = new unsigned int[outputs.counts.size()];
  double *times = new double[processes];
  double *memory = new double[processes];
  MPI_Barrier(MPI_COMM_WORLD);
  communicateResult(outputs, times, memory, output_values, my_time, memusage);


  printPCResults(myanalysis, outputs, output_values, times, memory);


  /* all done */
  MPI_Finalize();
  delete[] output_values;
  delete[] times;

}


