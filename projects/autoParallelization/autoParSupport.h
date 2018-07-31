#ifndef auto_par_support_INCLUDED
#define auto_par_support_INCLUDED

//Common headers for a ROSE translator
//#include <rose.h>
#include "LivenessAnalysis.h"
#include "CommandOptions.h"

//OpenMP attribute for OpenMP 3.0
#include "OmpAttribute.h"

//Array Annotation headers
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>

//Dependence graph headers
#include <AstInterface_ROSE.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <LoopTreeDepComp.h>

//Variable classification support
#include "DefUseAnalysis.h"


//Other standard C++ headers
#include <vector>
#include <string>
#include <map>


#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace AutoParallelization
{
  //Handle annotation, debugging flags, no longer used, replaced by using Sawyer command line processing
  //  void autopar_command_processing(std::vector<std::string>&argvList);

  // Required analysis and their initialization
  extern DFAnalysis * defuse;
  extern LivenessAnalysis* liv;
  extern bool enable_debug; // maximum debugging output to the screen
  extern bool keep_going; 
  extern bool enable_verbose;  // verbose mode, print out loop info. for successful or failed parallelization attempts . Default is true
  extern bool no_aliasing;  // assuming aliasing or not
  extern bool enable_patch; // an option to control the generation of patch files
  extern bool enable_diff; // an option to compare user-defined OpenMP pragmas to compiler generated ones.
  extern bool enable_modeling; // an experimental flag to turn on cost modeling being developed.
  extern bool b_unique_indirect_index; // assume all arrays used as indirect indices has unique elements(no overlapping)
  extern bool enable_distance; // print out absolute dependence distance for a dependence relation preventing from parallelization
  extern bool dump_annot_file; // print out annotation file's content
  extern std::vector<std::string> annot_filenames;

  extern bool keep_c99_loop_init; // avoid normalize C99 style loop init statement: for (int i=0; ...)
  // Conduct necessary analyses on the project, can be called multiple times during program transformations. 
  bool initialize_analysis(SgProject* project=NULL,bool debug=false);

  //------------ this section supports modeling of loop execution cost
  // software information for the loop
  typedef struct loopInfo Loop_Info; 
  typedef struct hardwareInfo Hardware_Info; 
  struct loopInfo {
    // arithmetic intensity: flops per byte
    float arithmetic_intensity; 
    // How many loop iterations estimated for this loop
    int iteration_count; 
    int flops_per_iteration; // floating point operations per iteration TODO double vs. single
  };

  // hardware info. generic for both CPUs and GPUs so far
  struct hardwareInfo {
    float main_mem_bandwidth; // theoretical peak bandwidth GBytes/second by default
    float main_mem_bandwidth_measured; // measured peak bandwidth through some microbenchmarks
    float peak_flops; // theoretical peak, GFlops/second by default
    float peak_flops_measured; // GFlops/second by default, measured peak flops through some microbenchmarks
  };

  // A baseline roofline model
  //! Using software and hardware information to estimate execution time in seconds 
  double rooflineModeling(Loop_Info *l, Hardware_Info *h);

  #if 0 // This is difficult to use: we have to differentiate types when storing them
   // Lazy method: store values as strings. Interpret them during usage. 
  //A hardware feature can be different types
  union HValue {
    int iv; 
    float fv;
    char* sv; 
  };
  #endif 


  class CSVReader // Reading CSV file into vector of vectors
  {
    // public interface   
    public:
      // Parse the input file and store data internally.
      CSVReader (std::string fname);

      std::vector <std::vector <std::string> > getResult()
      {return csv_table; }

      void prettyPrintResult();
      // Using hardware model number, store map of key-value pairs
     // All types of values are stored as strings. Users have to interprete them to different types (int, string, float, etc.) based on meanings of keys.
      static std::map < std::string,  std::map <std::string, std::string>  > hardwareDataBase; 

    private:
      std::string file_name;
      std::vector <std::vector <std::string> > csv_table;
      // Read and parse a line of a CSV stream
      // Store cells into a vector of strings: 
      //std::vector<std::string> readNextRow(std::istream istr)
      std::istream& readNextRow(std::istream& istr, std::vector<std::string> & result);
      // keep track of the cell number for each row.
      static int cell_counter;
      static void outputVectorElement(std::string s);
      static void outputVector(std::vector <std::string> str_vec );

      // read one entire CSV file, return vector of vectors of strings.
      std::vector <std::vector <std::string> > readCSVFile (std::string filename);
  };

  //----------------------end of the cost modeling section ------------------------
  //Release the resources for analyses
  void release_analysis();

  // Return the loop invariant of a canonical loop, return NULL otherwise
  SgInitializedName* getLoopInvariant(SgNode* loop);
  
  //Compute dependence graph for a loop, using ArrayInterface and ArrayAnnoation
  LoopTreeDepGraph* ComputeDependenceGraph(SgNode* loop, ArrayInterface*, ArrayAnnotation* annot);
  
  // Get the live-in and live-out variable sets for a for loop, recomputing liveness analysis if requested (useful after program transformation)
  void GetLiveVariables(SgNode* loop, std::vector<SgInitializedName*> &liveIns,
                      std::vector<SgInitializedName*> &liveOuts,bool reCompute=false);

  // Collect visible referenced variables within a scope (mostly a loop). 
  // Ignoring local variables declared within the scope. 
  // Specially recognize nested loops' invariant variables, which are candidates for private variables
  void CollectVisibleVaribles(SgNode* loop, std::vector<SgInitializedName*>& resultVars,std::vector<SgInitializedName*>& loopInvariants, bool scalarOnly=false);

  //! Collect a loop's variables which cause any kind of dependencies. Consider scalars only if requested.
  void CollectVariablesWithDependence(SgNode* loop, LoopTreeDepGraph* depgraph,std::vector<SgInitializedName*>& resultVars,bool scalarOnly=false);

  // Variable classification for a loop node 
  // Collect private, firstprivate, lastprivate, reduction and save into attribute
  void AutoScoping(SgNode *sg_node, OmpSupport::OmpAttribute* attribute, LoopTreeDepGraph* depgraph);

  // Recognize reduction variables for a loop
  // Refactored into SageInterface
 // std::vector<SgInitializedName*> RecognizeReduction(SgNode *sg_node, OmpSupport::OmpAttribute* attribute, std::vector<SgInitializedName*>& candidateVars); 

  // Collect all classified variables from an OmpAttribute attached to a loop node,regardless their omp type
  void CollectScopedVariables(OmpSupport::OmpAttribute* attribute, std::vector<SgInitializedName*>& result);

  // Eliminate irrelevant dependencies for a loop node 'sg_node'
  // Save the remaining dependencies which prevent parallelization
  void DependenceElimination(SgNode* sg_node, LoopTreeDepGraph* depgraph, std::vector<DepInfo>&remain, OmpSupport::OmpAttribute* attribute, 
       std::map<SgNode*, bool> & indirectTable, ArrayInterface* array_interface=0, ArrayAnnotation* annot=0);

#if 0 // refactored into the OmpSupport namespace
  //Generate and insert OpenMP pragmas according to OmpAttribute
  void generatedOpenMPPragmas(SgNode* node);
#endif
  //Parallelize an input loop at its outermost loop level, return true if successful
  bool ParallelizeOutermostLoop(SgNode* loop, ArrayInterface* array_interface, ArrayAnnotation* annot);

  //! Generate patch files for the introduced OpenMP pragmas (represented as OmpAttribute)
  void generatePatchFile(SgSourceFile* sfile);

  //! Output the difference between user-defined OpenMP and compiler-generated OpenMP
  void diffUserDefinedAndCompilerGeneratedOpenMP(SgSourceFile* sfile);

  //! Check if two expressions access different memory locations. If in double, return false
  //This is helpful to exclude some dependence relations involving two obvious different memory location accesses
  //TODO: move to SageInterface when ready
  bool differentMemoryLocation(SgExpression* e1, SgExpression* e2);

  //! Check if a loop has any unsupported language features so we can skip them for now
  bool useUnsupportedLanguageFeatures(SgNode* loop, VariantT* blackConstruct);

} //end namespace

#endif //auto_par_support_INCLUDED
