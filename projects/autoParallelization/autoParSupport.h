#ifndef auto_par_support_INCLUDED
#define auto_par_support_INCLUDED
#include <rose.h>

#include "CommandOptions.h"
// OpenMP attribute for OpenMP 3.0
#include "OmpAttribute.h"

//Array Annotation headers
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>

//dependence graph headers
#include <AstInterface_ROSE.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <LoopTreeDepComp.h>

//Variable classification support
#include "DefUseAnalysis.h"
#include "LivenessAnalysis.h"

// Other standard C++ headers
#include <vector>
#include <string>
#include <iostream>
namespace AutoParallelization
{
  //Handle annotation, debugging flags
  void autopar_command_processing(std::vector<std::string>&argvList);

  // Required analysis and their initialization
  extern DFAnalysis * defuse;
  extern LivenessAnalysis* liv;
  bool initialize_analysis(SgProject* project,bool debug=false);
  //Release the resources for analyses
  void release_analysis();
  //Compute dependence graph for a loop, using ArrayInterface and ArrayAnnoation
  LoopTreeDepGraph* ComputeDependenceGraph(SgNode* loop, ArrayInterface*, ArrayAnnotation* annot);
  
  // Eliminate irrelevant dependencies for a loop node 'sg_node'
  // Save the remaining dependencies which prevent parallelization
  void DependenceElimination(SgNode* sg_node, LoopTreeDepGraph* depgraph, std::vector<DepInfo>&);

  //Generate and insert OpenMP pragmas according to OmpAttribute
  void generatedOpenMPPragmas(SgNode* node);

} //end namespace
#endif //auto_par_support_INCLUDED
