#include "parallel_compass.h"

#include <mpi.h>
#include <limits>

#include "LoadSaveAST.h"
#include "../DistributedMemoryAnalysis/DistributedMemoryAnalysis.h"

#define DEBUG_OUTPUT false
#define DEBUG_OUTPUT_MORE false

// The pre-traversal runs before the distributed part of the analysis and is used to propagate context information down
// to the individual function definitions in the AST. Here, it just computes the depth of nodes in the AST.
class FunctionNamesPreTraversal: public AstTopDownProcessing<int>
{
protected:
  int evaluateInheritedAttribute(SgNode *node, int depth)
  {
    return depth + 1;
  }
};


// ************************************************************
// OUTPUT OBJECT: Initialize to zero and when error occurs, we increase the output
// ************************************************************
class CountingOutputObject: public Compass::OutputObject
{
public:
  CountingOutputObject(std::string name = ""): outputs(0), name(name){}
  virtual void addOutput(Compass::OutputViolationBase *) {outputs++; }
  void reset(){outputs = 0;}
  unsigned int outputs;
  std::string name;
};

//	Compass::OutputObject* output = new Compass::PrintingOutputObject(std::cerr); 

// ************************************************************
// What macro? There is no macro here. This is a code generator!
// ************************************************************
#define generate_checker(CheckerName)					\
  try {									\
    CompassAnalyses::CheckerName::Traversal *traversal;			\
    CountingOutputObject *output = new CountingOutputObject(#CheckerName); \
    traversal = new CompassAnalyses::CheckerName::Traversal(params, output); \
    traversals.push_back(traversal);					\
    bases.push_back(traversal);						\
    outputs.push_back(output);						\
  } catch (const Compass::ParameterNotFoundException &e) {		\
    std::cerr << e.what() << std::endl;					\
  }


// ************************************************************
// This is a description of all checkers.
// ************************************************************
void compassCheckers(std::vector<AstSimpleProcessing *> &traversals,
		     std::vector<Compass::TraversalBase *> &bases,
		     //        std::vector<Compass::OutputObject *> &outputs)
		     std::vector<CountingOutputObject *> &outputs) {
  try {
    Compass::Parameters params("compass_parameters");
#include "generate_checkers.C"
  } catch (const Compass::ParseError &e) {
    std::cerr << e.what() << std::endl;
  }
  //    outputs.push_back(new Compass::PrintingOutputObject(std::cerr));
}


// ************************************************************
// Time Measurement
// ************************************************************
double timeDifference(struct timespec end, struct timespec begin) {
  return (end.tv_sec + end.tv_nsec / 1.0e9)
    - (begin.tv_sec + begin.tv_nsec / 1.0e9);
}
inline void gettime(struct timespec &t) {
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
}


// ************************************************************
// output all the results
// ************************************************************
void output_results(std::vector<CountingOutputObject *> &outputs) {
  std::vector<CountingOutputObject *>::iterator o_itr;
  std::cout << "results: " << std::endl;
  for (o_itr = outputs.begin(); o_itr != outputs.end(); ++o_itr) {
    std::cout << (*o_itr)->name << " : " << (*o_itr)->outputs << " " << std::endl;
    (*o_itr)->reset();
  }
}


// ************************************************************
// NodeCounter to determine on how to split the nodes
// ************************************************************
class NodeCounter: public AstSimpleProcessing
{
public:
  size_t totalNodes;
  size_t totalFunctions;
  size_t *fileNodes;

  NodeCounter(int numberOfFiles)
    : totalNodes(0), totalFunctions(0), 
      fileNodes(new size_t[numberOfFiles+1]), fileptr(fileNodes) {
    *fileptr = 0;
  }

protected:
  // increase the number of nodes and nodes in files
  virtual void visit(SgNode *node) {
    totalNodes++;
    ++*fileptr;

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
  NodeCounter nc(project->numberOfFiles());
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



// ************************************************************
// check for the usage parameters
// ************************************************************
bool containsArgument(int argc, char** argv, char* pattern) {
  for (int i = 1; i < argc ; i++) {
    if (!strcmp(argv[i], pattern)) {
      return true;
    }
  }
  return false;
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


// ************************************************************
// main function
// ************************************************************
int main(int argc, char **argv)
{
  struct timespec begin, end;
  bool loadAST =false;
  bool saveAST =false;

  /* setup MPI */
  int my_rank, processes;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processes);


  if (my_rank == 0) {
    if (argc < 2)
      {
	std::cerr << "USAGE: "  << std::endl;
	std::cerr << "   executable filenames_in \t\t\truns on a project given a the specified file"  << std::endl;
	std::cerr << "   executable -save filename_out filenames_in \truns on the specified file and saves it to ast.ast"  << std::endl;
	std::cerr << "   executable -load filename_in \t\tloads the specified AST and runs the project"  << std::endl;
	std::cerr << std::endl;
	exit(0);
      }
  }

  if (containsArgument(argc, argv, "-load")) {
    loadAST = true;
  } else if (containsArgument(argc, argv, "-save")) {
    saveAST = true;
  } 

  SgProject *root = NULL;
  /* read the AST, either from a binary file or from sources */
  if (saveAST) {
    if (my_rank == 0) {
      gettime(begin);
      if (argc>3) {
	char** argv2 = new char*[argc];
	argv2[0] = argv[0];
	std::cout << " i: 0 argv[0] " << argv[0] << std::endl;
	for (int i=3; i<argc; i++) {
	  argv2[i-2] = argv[i];
	  std::cout << " i: " << (i-2) << " argv[i] " << argv2[i-2] << std::endl;
	}
	root = frontend(argc-2, argv2);
	std::string out_filename = argv[2];//"ast.ast";
	LoadSaveAST::saveAST(out_filename, root); 
      }
      gettime(end);
      exit(0);
    }
  } else if (loadAST) {
    std::cout << "ROSE loading .... " << argv[2] << std::endl;
    gettime(begin);
    root = LoadSaveAST::loadAST(argv[2]); 
    gettime(end);
  } 

  if (!saveAST && !loadAST) {
    gettime(begin);
    std::cout << "ROSE frontend .... " << std::endl;
    root = frontend(argc, argv);
    gettime(end);
  }

  ROSE_ASSERT(root);




  /* setup checkers */
  std::vector<AstSimpleProcessing *> traversals;
  std::vector<AstSimpleProcessing *>::iterator t_itr;
  std::vector<Compass::TraversalBase *> bases;
  std::vector<Compass::TraversalBase *>::iterator b_itr;
  //    std::vector<Compass::OutputObject *> outputs;
  //std::vector<Compass::OutputObject *>::iterator o_itr;
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


#define RUN_ON_FILES 0
#define CALL_RUN_FOR_CHECKERS 1
#define RUN_COMBINED_CHECKERS 0
#define GERGO_ALGO 0

  /* traverse the files */
  gettime(begin);
  double memusage_b = ROSE_MemoryUsage().getMemoryUsageMegabytes();

#if CALL_RUN_FOR_CHECKERS
  if (my_rank==0)
  std::cout << "\n>>> Running in sequence ... " << std::endl;
#endif

#if RUN_ON_FILES
  std::cout << "\n>>> Running on files ... " << std::endl;
  /* figure out which files to process on this process */
  std::pair<int, int> bounds = computeFileIndices(root, my_rank, processes);
  for (int i = bounds.first; i < bounds.second; i++)
    {
      std::cout << "bounds ("<< i<<" [ " << bounds.first << "," << bounds.second << "[ of " << std::endl;
 #if CALL_RUN_FOR_CHECKERS
      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
	if (DEBUG_OUTPUT_MORE) 
	  std::cout << "running checker (" << i << " in ["<< bounds.first << "," << bounds.second 
		    <<"[) : " << (*b_itr)->getName() << " \t on " << (root->get_file(i).getFileName()) << std::endl; 
	(*b_itr)->run(&root->get_file(i));
      }

 #elif RUN_COMBINED_CHECKERS
      std::cout << "\n>>> Running combined ... " << std::endl;
      AstCombinedSimpleProcessing combined(traversals);
      combined.traverse(&root->get_file(i), preorder);
 #else
  int nrOfThreads = 5;
  std::cout << "\n>>> Running shared ... with " << nrOfThreads << " threads per traversal " << std::endl;
      AstSharedMemoryParallelSimpleProcessing parallel(traversals,nrOfThreads);
      parallel.traverseInParallel(&root->get_file(i), preorder);
 #endif
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
  std::vector<int> bounds;
  myanalysis.computeFunctionIndicesPerNode(root, bounds, initialDepth, &preTraversal);
  for (int i = 0; i<(int)bounds.size();i++) {
    if (bounds[i]== my_rank) {
      //std::cout << "bounds ("<< i<<"/"<< bounds.size()<<")  - weight: " << (myanalysis.myNodeCounts[i]*
      //									     myanalysis.myFuncWeights[i]) << std::endl;
      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
	if (DEBUG_OUTPUT_MORE) 
	  std::cout << "running checker (" << i << ") : " << (*b_itr)->getName() << " \t on function: " << 
	    (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_name().str()) << 
	    "     in File: " << 	(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]->get_file_info()->get_filename()) 
		    << std::endl; 

	(*b_itr)->run(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
      }
    }
  }
#endif // own algo
 #elif RUN_COMBINED_CHECKERS
      std::cout << "\n>>> Running combined ... " << std::endl;
      AstCombinedSimpleProcessing combined(traversals);
      combined.traverse(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i], preorder);
 #else
  int nrOfThreads = 5;
  std::cout << "\n>>> Running shared ... with " << nrOfThreads << " threads per traversal " << std::endl;
      AstSharedMemoryParallelSimpleProcessing parallel(traversals,nrOfThreads);
      parallel.traverseInParallel(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i], preorder);
#endif // run for checkers


#endif // run on files


  gettime(end);
  double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  double memusage = memusage_e-memusage_b;
  double my_time = timeDifference(end, begin);
  std::cout << ">>> Process is done. Time: " << my_time << "  Memory: " << memusage << " MB." << std::endl;

  /* communicate results */
  unsigned int *my_output_values = new unsigned int[outputs.size()];
  for (size_t i = 0; i < outputs.size(); i++)
    my_output_values[i] = outputs[i]->outputs;

  unsigned int *output_values = new unsigned int[outputs.size()];

  MPI_Reduce(my_output_values, output_values, outputs.size(), MPI_UNSIGNED,
	     MPI_SUM, 0, MPI_COMM_WORLD);


  /* communicate times */
  double *times = new double[processes];
  MPI_Gather(&my_time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0,
	     MPI_COMM_WORLD);

  double *memory = new double[processes];
  MPI_Gather(&memusage, 1, MPI_DOUBLE, memory, 1, MPI_DOUBLE, 0,
	     MPI_COMM_WORLD);

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
      std::cout << "processor: " << i << " time: " << times[i] << "  memory: " << memory[i] <<  " MB " << std::endl;
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
#if RUN_ON_FILES
	      << "\n    fastest process: " << min_time << " fastest   in file: " << root->get_file(fastest_func).getFileName() 
	      << "\n    slowest process: " << max_time << " slowest   in file: " << root->get_file(slowest_func).getFileName()
#else
	      << "\n    fastest process: " << min_time << " fastest func: " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[fastest_func]->get_name().str())
              << "  in File : " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[fastest_func]->get_file_info()->get_filename())
	      << "\n    slowest process: " << max_time << " slowest func: " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[slowest_func]->get_name().str())
              << "  in File : " << (myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[slowest_func]->get_file_info()->get_filename())
#endif
	      << std::endl;
    std::cout << std::endl;

    std::cout <<  "The total amount of files is : " << root->numberOfFiles() << std::endl;
#if RUN_ON_FILES
#else
    std::cout <<  "The total amount of functions is : " << myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls.size() << std::endl;
    std::cout <<  "The total amount of nodes is : " << myanalysis.DistributedMemoryAnalysisBase<int>::nrOfNodes << std::endl;
#endif
  }


  /* all done */
  MPI_Finalize();
  delete[] output_values;
  delete[] times;

}


