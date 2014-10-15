
// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

#include "shiftCalculusCompiler.h"

InheritedAttribute::InheritedAttribute()
   {
   }

InheritedAttribute::InheritedAttribute( const InheritedAttribute & X )
   {
   }



SynthesizedAttribute::SynthesizedAttribute()
   {
   }

SynthesizedAttribute::SynthesizedAttribute( const SynthesizedAttribute & X )
   {
   }

Traversal::Traversal()
   {
   }


#define DEBUG_USING_DOT_GRAPHS 0

int main( int argc, char * argv[] )
   {
  // If we want this translator to take specific options (beyond those defined 
  // by ROSE) then insert command line processing for new options here.

  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

#if DEBUG_USING_DOT_GRAPHS
  // generateDOTforMultipleFile(*project);
     generateDOT(*project,"_before_transformation");
#endif
#if DEBUG_USING_DOT_GRAPHS
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 12000;
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_before");
#endif

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

  // Define the traversal
     Traversal shiftCalculus_Traversal;

#if 1
     printf ("Call the traversal starting at the project (root) node of the AST \n");
#endif

  // Call the traversal starting at the project (root) node of the AST
     SynthesizedAttribute result = shiftCalculus_Traversal.traverse(project,inheritedAttribute);

#if 1
     printf ("DONE: Call the traversal starting at the project (root) node of the AST \n");
#endif

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

#if DEBUG_USING_DOT_GRAPHS
     printf ("Write out the DOT file after the transformation \n");
     generateDOTforMultipleFile(*project,"after_transformation");
     printf ("DONE: Write out the DOT file after the transformation \n");
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
  // const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_after");
#endif

  // Regenerate the source code but skip the call the to the vendor compiler.
     return backend(project);
   }

