
// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

#include "stencilAndStencilOperatorDetection.h"
#include "stencilEvaluation.h"

#include "shiftCalculusCompiler.h"

#include "dslCodeGeneration.h"

#define DEBUG_USING_DOT_GRAPHS 1

int main( int argc, char * argv[] )
   {
  // If we want this translator to take specific options (beyond those defined 
  // by ROSE) then insert command line processing for new options here.

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
     SgProject* project = frontend(argc,argv,frontendConstantFolding);
     ROSE_ASSERT(project != NULL);

  // DQ (2/8/2015): Find the associated SgFile so we can restrict processing to the current file.
     ROSE_ASSERT(project->get_fileList().empty() == false);
     SgFile* firstFile = project->get_fileList()[0];
     ROSE_ASSERT(firstFile != NULL);

#if DEBUG_USING_DOT_GRAPHS
  // generateDOTforMultipleFile(*project);
  // generateDOT(*project,"_before_transformation");
     AstDOTGeneration astdotgen;
     astdotgen.generateWithinFile(firstFile,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,"_before_transformation");
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 12000;
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_before");
#endif

  // Build the inherited attribute
     Detection_InheritedAttribute inheritedAttribute;

  // Define the traversal
     DetectionTraversal shiftCalculus_DetectionTraversal;

#if 1
     printf ("Call the Detection traversal starting at the project (root) node of the AST \n");
#endif

  // Call the traversal starting at the project (root) node of the AST
  // Detection_SynthesizedAttribute result = shiftCalculus_DetectionTraversal.traverse(project,inheritedAttribute);
     Detection_SynthesizedAttribute result = shiftCalculus_DetectionTraversal.traverseWithinFile(firstFile,inheritedAttribute);
#if 1
     printf ("Stencil Operator was transformed: %s \n",result.get_stencilOperatorTransformed() ? "true" : "false");
#endif
     ROSE_ASSERT(result.get_stencilOperatorTransformed() == false);

#if 1
     printf ("DONE: Call the Detection traversal starting at the project (root) node of the AST \n");
#endif
#if 1
     shiftCalculus_DetectionTraversal.display();
#endif
#if 1
     printf ("Exiting after the initial traversal to detect the stencil useage. \n");
     ROSE_ASSERT(false);
#endif

  // Build the inherited attribute
     StencilEvaluation_InheritedAttribute inheritedAttribute_stencilEval;

  // Define the traversal
     StencilEvaluationTraversal shiftCalculus_StencilEvaluationTraversal(shiftCalculus_DetectionTraversal);

#if 1
     printf ("Call the StencilEvaluation traversal starting at the project (root) node of the AST \n");
#endif

  // Call the traversal starting at the project (root) node of the AST
  // StencilEvaluation_SynthesizedAttribute result_stencilEval = shiftCalculus_StencilEvaluationTraversal.traverse(project,inheritedAttribute_stencilEval);
     StencilEvaluation_SynthesizedAttribute result_stencilEval = shiftCalculus_StencilEvaluationTraversal.traverseWithinFile(firstFile,inheritedAttribute_stencilEval);
#if 1
     printf ("Stencil Evaluation was transformed: %s \n",result_stencilEval.get_stencilOperatorTransformed() ? "true" : "false");
#endif
     ROSE_ASSERT(result_stencilEval.get_stencilOperatorTransformed() == false);

#if 1
     printf ("DONE: Call the StencilEvaluation traversal starting at the project (root) node of the AST \n");
#endif

     shiftCalculus_StencilEvaluationTraversal.displayStencil("After evaluation of stencil");

#if 1
     printf ("Call generateStencilCode to generate example code \n");
#endif

#if 1
     printf ("Exiting after the second traversal to evaluate the stencils. \n");
     ROSE_ASSERT(false);
#endif

  // Generate code from stencil data structure.
     bool generateLowlevelCode = true;
     generateStencilCode(shiftCalculus_StencilEvaluationTraversal,generateLowlevelCode);

#if 1
     printf ("DONE: Call generateStencilCode to generate example code \n");
#endif

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

#if DEBUG_USING_DOT_GRAPHS
     printf ("Write out the DOT file after the transformation \n");
  // generateDOTforMultipleFile(*project,"after_transformation");
     generateDOT(*project,"_after_transformation");
     printf ("DONE: Write out the DOT file after the transformation \n");
#endif
#if DEBUG_USING_DOT_GRAPHS && 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
  // const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_after");
#endif

  // Regenerate the source code but skip the call the to the vendor compiler.
     return backend(project);
   }

