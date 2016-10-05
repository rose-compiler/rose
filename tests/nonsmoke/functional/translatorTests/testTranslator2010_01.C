// This is a translator that demonstrates (or demonstrated) a bug in ROSE.
// This is is provided as part of regression tests on translators that 
// demonstrate bugs in ROSE and is different from the tests/nonsmoke/functional/CompileTests
// directory which demonstrates input codes that demonstrate bugs in ROSE.

#include "rose.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

int
main(int argc, char **argv)
   {
     SgFile *file = buildFile("blank.cpp", "out.cpp");
     ROSE_ASSERT(file != NULL);
#if 1
     SgSourceFile *outputFile = isSgSourceFile(file);
     ROSE_ASSERT(outputFile != NULL);
     SgProject *project = outputFile->get_project();
     ROSE_ASSERT(project != NULL);
     SgGlobal *globalScope = outputFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);
#endif

#if 1
     SgFunctionDeclaration *func = buildDefiningFunctionDeclaration("a_function", buildVoidType(), buildFunctionParameterList(), globalScope);
     appendStatement(func, globalScope);
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
     generateDOT ( *project );
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

     AstTests::runAllTests(project);
     project->unparse();

     return 0;
   }
