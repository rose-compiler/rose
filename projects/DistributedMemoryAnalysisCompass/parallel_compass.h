#ifndef PARALLEL_COMPASS_H
#define PARALLEL_COMPASS_H

//#include <mpi.h>


#include "AstSharedMemoryParallelProcessing.h"

#include "compass.h"
#include "checkers.h"

#include <limits>

#include "LoadSaveAST.h"
#include "DistributedMemoryAnalysis.h"

#include "DefUseAnalysis.h"
#include "DefUseAnalysis_perFunction.h"

#ifdef _OPENMP
  #include <omp.h>
#endif

bool loadAST =false;
bool saveAST =false;
bool loadDFA =false;
bool saveDFA =false;
SgProject *root = NULL;
struct timespec begin_time_checker, end_time_checker;
struct timespec begin_time, end_time;
struct timespec begin_init_time, end_init_time;
struct timespec begin_time_node, end_time_node;
struct timespec begin_time_0, end_time_0;
struct timespec begin_time_defuse, end_time_defuse;
//int my_rank, processes;
bool sequential=false;
bool combined=false;
int nrOfThreads = 3;
int* dynamicFunctionsPerProcessor;

// (tps 07/24/08) added to support dataflowanalysis
// before compass is started.
DFAnalysis* defuse;

// prototype. Implementation is in Compass.
void
buildCheckers( std::vector<const Compass::Checker*> &retVal, Compass::Parameters &params, 
               Compass::OutputObject &output, SgProject* pr );


// ************************************************************
// check for the usage parameters
// ************************************************************
bool containsArgument(int argc, char** argv, std::string pattern) {
  for (int i = 1; i < argc ; i++) {
    //    if (!strcmp(argv[i], pattern)) {
    std::string argument = argv[i];
    if (argument== pattern) {
      return true;
    }
  }

  return false;
}


void initPCompass(int argc, char **argv, int processes) {
  if (Compass::my_rank == 0) {
    if (argc < 2)
      {
	std::cerr << "USAGE: "  << std::endl;
	std::cerr << "   executable filenames_in \t\t\truns on a project given a the specified file"  << std::endl;
	std::cerr << "   executable -save filename_out filenames_in \truns on the specified file (filenames_in) and saves it to filename_out.ast"  << std::endl;
	std::cerr << "   executable -load filename_in -sdfa \truns dataflow analysis (in parallel) and saves the defuse result to the same output filename.data"  << std::endl;
	std::cerr << "   executable -load filename_in \t\tloads the specified AST and runs the project"  << std::endl;
	std::cerr << "   executable -load filename_in -ldfa \t\tloads the dataflow analysis for the same input filename.data" << std::endl;
	std::cerr << "   executable [-combined | -shared ] (sequential is default)"  << std::endl;
	std::cerr << std::endl;
	exit(0);
      }
  }

  if (containsArgument(argc, argv, "-load")) {
    if (containsArgument(argc, argv, "-ldfa")) {
      loadDFA = true;
    }
    if (containsArgument(argc, argv, "-sdfa")) {
      saveDFA = true;
    }
    loadAST = true;
  } else if (containsArgument(argc, argv, "-save")) {
    if (containsArgument(argc, argv, "-sdfa")) {
      saveDFA = true;
    }
    saveAST = true;
  } 

  if (containsArgument(argc, argv, "-combined")) {
    sequential=false;
    combined=true;
  } else 
  if (containsArgument(argc, argv, "-shared")) {
    sequential=false;
    combined=false;
  } else {
    sequential=true;
  }

  /* read the AST, either from a binary file or from sources */
  if (saveAST) {
    std::cerr << "ROSE saving FILE.... " << argv[2] << std::endl;
    if (processes>1) {
      std::cerr << "Cant save AST with more than one processor" << std::endl;
    }
    if (Compass::my_rank == 0) {
      Compass::gettime(begin_time);
      if (argc>3) {
	char** argv2 = new char*[argc];
	argv2[0] = argv[0];
	std::cout << " i: 0 argv[0] " << argv[0] << std::endl;
	for (int i=3; i<argc; i++) {
	  argv2[i-2] = argv[i];
	  std::cout << " i: " << (i-2) << " argv["<<i<<"] " << argv2[i-2] << std::endl;
	}
      
	root = frontend(argc-2, argv2);
	std::string out_filename = argv[2];//"ast.ast";
	// this can only run in sequence!
	if (saveDFA) {
	  std::string name = argv[2];
	  name.append(".data");
	  std::cerr << "ROSE saving DFA enabled.... " << name << std::endl;
	  Compass::saveDFA(name,root);
	}

	LoadSaveAST::saveAST(out_filename, root); 
      }
      Compass::gettime(end_time);
      MPI_Finalize();
      exit(0);
    }
  } else if (loadAST) {
    Compass::gettime(begin_time);
    std::cerr << "ROSE loading FILE.... " << argv[2] << std::endl;
    root = LoadSaveAST::loadAST(argv[2]); 
    Compass::gettime(end_time);
    if (loadDFA) {
      std::string name = argv[2];
      name.append(".data");
      std::cerr << "ROSE loading DFA enabled.... " << name << std::endl;
      Compass::loadDFA(name,root);
      //MPI_Finalize();
      //exit(0);
    } else if  (saveDFA) {
      // this can run in parallel!
      std::string name = argv[2];
      name.append(".data");
      std::cerr << "ROSE saving DFA enabled .... " << name << std::endl;
      Compass::saveDFA(name,root);
      std::cerr << Compass::my_rank << ": ROSE saving DFA done.  " <<  std::endl;

      MPI_Finalize();
      exit(0);
    }
  } else {
    std::cerr << "No Load or Save .... " << std::endl;
  }

  if (!saveAST && !loadAST) {
    Compass::gettime(begin_time);
    std::cout <<"  ROSE frontend .... " << std::endl;
    root = frontend(argc, argv);
    Compass::gettime(end_time);
  }

}

#if 0
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
#endif

// ************************************************************
// OUTPUT OBJECT: Initialize to zero and when error occurs, we increase the output
// ************************************************************
class CountingOutputObject: public Compass::OutputObject
{
public:
  CountingOutputObject(){
    
  }
  virtual void addOutput(Compass::OutputViolationBase *obj) {
    ++counts[obj->getCheckerName()];
    // tps : this line prints the violations for the parallel checkers.
    std::cout << obj->getString() << std::endl;
   }
  void fillOutputList(  std::vector<const Compass::Checker *> bases) {
    std::vector<const Compass::Checker *>::iterator it = bases.begin();
    for (;it!=bases.end();++it) {
      const Compass::Checker* checker = *it;
      counts[checker->checkerName]=0;
    }
  }
  std::map<std::string, unsigned int> counts;
};



//	Compass::OutputObject* output = new Compass::PrintingOutputObject(std::cerr); 

// ************************************************************
// What macro? There is no macro here. This is a code generator!
// ************************************************************
#define generate_checker(checkerName)					\
  try {									\
    const Compass::CheckerUsingAstSimpleProcessing* const checker = dynamic_cast<Compass::CheckerUsingAstSimpleProcessing*>(checkerName##Checker);      \
    CountingOutputObject *output = new CountingOutputObject(#checkerName); \
    if (checker) {                                                      \
      AstSimpleProcessing *traversal = checker->createSimpleTraversal(params, output);			\
      traversals.push_back(traversal);					\
      bases.push_back(traversal);					\
      outputs.push_back(output);					\
    }                                                                   \
  } catch (const Compass::ParameterNotFoundException &e) {		\
    std::cerr << e.what() << std::endl;					\
  }



// ************************************************************
// output all the results
// ************************************************************
void output_results(CountingOutputObject * &outputs) {
  std::map<std::string, unsigned int> out = outputs->counts;
  std::map<std::string, unsigned int> ::iterator o_itr;
  std::cout << "results: " << std::endl;
  for (o_itr = out.begin(); o_itr != out.end(); ++o_itr) {
    std::cout << (*o_itr).first << " : " << (*o_itr).second << " " << std::endl;
  }
}


void communicateResult(CountingOutputObject  &outputs, 
                       double* times, double* memory, 
		       unsigned int* output_values, 
		       double my_time, double memusage) {
  /* communicate results */
  unsigned int *my_output_values = new unsigned int[outputs.counts.size()];
  std::map<std::string, unsigned int> ::iterator o_itr;
  int i=0;
  for (o_itr = outputs.counts.begin(); o_itr != outputs.counts.end(); ++o_itr,++i) {
    my_output_values[i] = o_itr->second;
  }

  MPI_Reduce(my_output_values, output_values, outputs.counts.size(), MPI_UNSIGNED,
             MPI_SUM, 0, MPI_COMM_WORLD);

  /* communicate times */
  MPI_Gather(&my_time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  MPI_Gather(&memusage, 1, MPI_DOUBLE, memory, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);
}

void communicateResult(CountingOutputObject  &outputs, 
                       double* times, double* memory, 
		       unsigned int* output_values, 
		       double my_time, double memusage, double* nr_func, double thisfunction) {
  /* communicate results */
  unsigned int *my_output_values = new unsigned int[outputs.counts.size()];
  std::map<std::string, unsigned int> ::iterator o_itr;
  int i=0;
  for (o_itr = outputs.counts.begin(); o_itr != outputs.counts.end(); ++o_itr,++i) {
    my_output_values[i] = o_itr->second;
  }

  MPI_Reduce(my_output_values, output_values, outputs.counts.size(), MPI_UNSIGNED,
             MPI_SUM, 0, MPI_COMM_WORLD);

  /* communicate times */
  MPI_Gather(&my_time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  MPI_Gather(&memusage, 1, MPI_DOUBLE, memory, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  MPI_Gather(&thisfunction, 1, MPI_DOUBLE, nr_func, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);
  
}


void communicateResult(CountingOutputObject  &outputs, 
                       double* times, double* memory, 
		       unsigned int* output_values, 
		       double my_time, double memusage, int* maxtime_nr, int max_time_i,  
		       double* maxtime_val, double maxtime,
		       double* calctimes, double calc_time_processor,
		       double* commtimes, double comm_time_processor
		       ) {
  /* communicate results */
  unsigned int *my_output_values = new unsigned int[outputs.counts.size()];
  std::map<std::string, unsigned int> ::iterator o_itr;
  int j=0;
  for (o_itr = outputs.counts.begin(); o_itr != outputs.counts.end(); ++o_itr, ++j) 
    my_output_values[j]=o_itr->second;


  MPI_Reduce(my_output_values, output_values, outputs.counts.size(), MPI_UNSIGNED,
             MPI_SUM, 0, MPI_COMM_WORLD);

  /* communicate times */
  MPI_Gather(&my_time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  MPI_Gather(&memusage, 1, MPI_DOUBLE, memory, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  MPI_Gather(&max_time_i, 1, MPI_INT, maxtime_nr, 1, MPI_INT, 0,
             MPI_COMM_WORLD);

  MPI_Gather(&maxtime, 1, MPI_DOUBLE, maxtime_val, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  MPI_Gather(&calc_time_processor, 1, MPI_DOUBLE, calctimes, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  MPI_Gather(&comm_time_processor, 1, MPI_DOUBLE, commtimes, 1, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);
  
}

#endif
