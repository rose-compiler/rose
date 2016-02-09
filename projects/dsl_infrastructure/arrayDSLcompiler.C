// This is the file that represents the array dsl compiler.
// It should be generic to any specific abstraction (but for 
// now it can be specific to the array DSL compiler).

#include <iostream>

#include "rose.h"

// #include "ompAstConstruction.h"

// #include "stencilAndStencilOperatorDetection.h"
// #include "stencilEvaluation.h"

//#include "shiftCalculusCompiler.h"
// #include "dslCodeGeneration.h"

#include "VariableIdMapping.h"

// This line can maybe be generic in the future.
// #include "dsl_attributes.h"
// #include "array_dsl_attributes.h"
#include "array.h"
#include "generated_dsl_attributes.h"

#include "dsl_detection.h"

#include "dslSupport.h"


using namespace std;

using namespace SPRAY;
using namespace SageInterface;
using namespace SageBuilder;

VariableIdMapping variableIdMapping;

// Control generation of DOT graphs of the AST for debugging.
#define DEBUG_USING_DOT_GRAPHS 1


int main( int argc, char * argv[] )
   {
  // If we want this translator to take specific options (beyond those defined 
  // by ROSE) then insert command line processing for new options here.

    std::vector <std::string> argvList (argv, argv + argc);

  // To better support the stencil specification that might benifit from constant 
  // folding, I have turned this ON is hte frontend.  By default it is OFF so that
  // we can preserve source code as much as possible (original expression trees).
  // The Stencil DSL can be made to work in eithr setting, but this make sure that
  // dimension dependent processing of the stencil coeficients will be evaluated 
  // to constants.  I will turn this off (and thus use a less blunt axe) when the new
  // constant expression evaluation in ROSE is fixed to support more general types
  // than integer expresion (should be done by JP later today).
  // bool frontendConstantFolding = true;
     bool frontendConstantFolding = false;

  // Generate the ROSE AST.
  // SgProject* project = frontend(argc,argv,frontendConstantFolding);
     SgProject* project = frontend(argvList,frontendConstantFolding);
     ROSE_ASSERT(project != NULL);

     try
        {
          variableIdMapping.computeVariableSymbolMapping(project);
        }
     catch(char* str)
        {
          cout << "*Exception raised: " << str << endl;
        } 
     catch(const char* str) 
        {
          cout << "Exception raised: " << str << endl;
        } 
     catch(string str)
        {
          cout << "Exception raised: " << str << endl;
        }

  // variableIdMapping.toStream(cout);

#if 1
     printf ("variableIdMapping.getVariableIdSet().size() = %zu \n",variableIdMapping.getVariableIdSet().size());
     ROSE_ASSERT(variableIdMapping.getVariableIdSet().size() > 0);
#endif

#if 0
     printf ("Exiting as a test after calling variableIdMapping.computeVariableSymbolMapping(project) \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("Calling constant folding \n");
     ConstantFolding::constantFoldingOptimization(project,false);

#if 0
     printf ("Exiting as a test after calling ConstantFolding::constantFoldingOptimization() \n");
     ROSE_ASSERT(false);
#endif
#endif

  // DQ (2/8/2015): Find the associated SgFile so we can restrict processing to the current file.
     ROSE_ASSERT(project->get_fileList().empty() == false);
     SgFile* firstFile = project->get_fileList()[0];
     ROSE_ASSERT(firstFile != NULL);

#if DEBUG_USING_DOT_GRAPHS
  // generateDOTforMultipleFile(*project);
     generateDOT(*project,"_before_transformation");
  // generateDOT_withIncludes(*project,"_before_transformation");
  // AstDOTGeneration astdotgen;
  // astdotgen.generateWithinFile(firstFile,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,"_before_transformation");
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 12000;
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_before");
#endif

  // Generate maps from generated DSL data structures.
     DSL_Support::outputGeneratedData();

  // Build the inherited attribute
     Detection_InheritedAttribute inheritedAttribute;

  // Define the traversal
  // DetectionTraversal shiftCalculus_DetectionTraversal;
     DetectionTraversal shiftCalculus_DetectionTraversal(project);

#if 1
     printf ("\n*************************************************************************** \n");
     printf ("Call the Detection traversal starting at the project (root) node of the AST \n");
     printf ("*************************************************************************** \n\n");
#endif

  // Call the traversal starting at the project (root) node of the AST
  // Detection_SynthesizedAttribute result = shiftCalculus_DetectionTraversal.traverse(project,inheritedAttribute);
     Detection_SynthesizedAttribute result = shiftCalculus_DetectionTraversal.traverseWithinFile(firstFile,inheritedAttribute);

#if 1
     printf ("\n********************************************************************************* \n");
     printf ("DONE: Call the Detection traversal starting at the project (root) node of the AST \n");
     printf ("********************************************************************************* \n\n");
#endif

     return 0;
   }
