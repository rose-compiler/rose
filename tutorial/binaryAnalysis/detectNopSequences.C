// This example ROSE translator just does an analysis of the input binary.
// The existence of NOP sequences are detected and reported.  For each
// NOP in the sequence the size of the NOP instruction is reported.
// Note that all multi-byte NOP instructions are detected and so the
// reported size of each instruction in the NOP sequence can vary.
// Intel multi-byte NOP instructions can be 1-9 bytes long.

#include "rose.h"
#include "detectNopSequencesTraversal.h"

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

     CountTraversal t;
     t.traverse(project,preorder);

  // regenerate the original executable.
     return backend(project);
   }
