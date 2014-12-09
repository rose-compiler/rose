// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

#include "operatorFiniteStateMachine.h"

#include "operatorEvaluation.h"

#include "mapleDSLCompiler.h"

#include "mapleOperatorAPI.h"

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
     bool frontendConstantFolding = true;

  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv,frontendConstantFolding);
     ROSE_ASSERT(project != NULL);

#if DEBUG_USING_DOT_GRAPHS
  // generateDOTforMultipleFile(*project);
     generateDOT(*project,"_before_transformation");
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 12000;
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_before");
#endif

  // Build the inherited attribute
     OperatorEvaluation_InheritedAttribute inheritedAttribute_operatorEval;

  // Define the traversal
     OperatorEvaluationTraversal operatorEvaluationTraversal;

#if 0
     printf ("Call the OperatorEvaluation traversal starting at the project (root) node of the AST \n");
#endif

  // Call the traversal starting at the project (root) node of the AST
     OperatorEvaluation_SynthesizedAttribute result_operatorEval = operatorEvaluationTraversal.traverse(project,inheritedAttribute_operatorEval);
#if 1
     printf ("Operator Evaluation was transformed: %s \n",result_operatorEval.get_operatorTransformed() ? "true" : "false");
#endif
  // ROSE_ASSERT(result_operatorEval.get_operatorTransformed() == true);

#if 0
     printf ("DONE: Call the OperatorEvaluation traversal starting at the project (root) node of the AST \n");
#endif

#if 0
     operatorEvaluationTraversal.displayOperator("After static (compile-time) evaluation of Maple DSL operators");
#if 0
     printf ("Call generateMapleCode to generate example code \n");
#endif
  // Generate code from stencil data structure.
     bool generateLowlevelCode = true;
     generateMapleCode(operatorEvaluationTraversal,generateLowlevelCode);
#if 0
     printf ("DONE: Call generateMapleCode to generate example code \n");
#endif
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

