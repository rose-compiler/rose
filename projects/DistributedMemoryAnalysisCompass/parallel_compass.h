#ifndef PARALLEL_COMPASS_H
#define PARALLEL_COMPASS_H

#include <mpi.h>
#include <rose.h>

#include "AstSharedMemoryParallelProcessing.h"

#include "compass.h"
#include "checkers.h"

#include <limits>

#include "LoadSaveAST.h"
#include "../DistributedMemoryAnalysis/DistributedMemoryAnalysis.h"

#include <mpi.h>

bool loadAST =false;
bool saveAST =false;
SgProject *root = NULL;
struct timespec begin_time, end_time;
int my_rank, processes;


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
// Time Measurement
// ************************************************************
double timeDifference(struct timespec end, struct timespec begin) {
  return (end.tv_sec + end.tv_nsec / 1.0e9)
    - (begin.tv_sec + begin.tv_nsec / 1.0e9);
}
inline void gettime(struct timespec &t) {
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
}


void initPCompass(int argc, char **argv) {
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

  /* read the AST, either from a binary file or from sources */
  if (saveAST) {
    if (my_rank == 0) {
      gettime(begin_time);
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
      gettime(end_time);
      exit(0);
    }
  } else if (loadAST) {
    std::cout << "ROSE loading .... " << argv[2] << std::endl;
    gettime(begin_time);
    root = LoadSaveAST::loadAST(argv[2]); 
    gettime(end_time);
  } 

  if (!saveAST && !loadAST) {
    gettime(begin_time);
    std::cout << "ROSE frontend .... " << std::endl;
    root = frontend(argc, argv);
    gettime(end_time);
  }

}

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


void communicateResult(std::vector<CountingOutputObject *> &outputs, 
		       double* times, double* memory, 
		       unsigned int* output_values, 
		       double my_time, double memusage) {
  /* communicate results */
  unsigned int *my_output_values = new unsigned int[outputs.size()];
  for (size_t i = 0; i < outputs.size(); i++)
    my_output_values[i] = outputs[i]->outputs;

  MPI_Reduce(my_output_values, output_values, outputs.size(), MPI_UNSIGNED,
	     MPI_SUM, 0, MPI_COMM_WORLD);

  /* communicate times */
  MPI_Gather(&my_time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0,
	     MPI_COMM_WORLD);

  MPI_Gather(&memusage, 1, MPI_DOUBLE, memory, 1, MPI_DOUBLE, 0,
	     MPI_COMM_WORLD);
}



#endif
