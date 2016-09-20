/*
// This file supports the new parse tree support in ROSE.
// specifically this is a "Concrete Syntax Augmented AST"
// Because the ROSE IR is close to that of the C/C++/Fortran 
// grammar the parse tree can be derived from the token stream
// and the AST.  The principal representation of the CSA AST
// is a map using the IR nodes of the AST as keys into the map
// and the map elements being a data structure (TokenStreamSequenceToNodeMapping)
// containing three pairs of indexes representing the subsequence 
// of tokens for the leading tokens (often white space), the token
// subsequence for the AST IR node (including its subtree), and
// the trailing token subsequence (often white space).

// So where the AST might be:
//        SgWhileStmt
//        /        \
// SgStatement  SgStatement
// (predicate)    (body)
//
// The associated parse tree would be:
//
//              SgWhileStmt
//        /  /      \      \      \
// "while" "(" SgStatement  ")" SgStatement
//               (predicate)      (body)
//
// (so much for ASCI art).
//

// We have a number of ways that we expect could be a problem for this 
// token stream mapping (possible failure modes):
//   1) Toky() macro to write code (not working yet)
//   2) Token pasting operator ## (WORKS)
//   3) Use equivalent of generated binary as a test for generate source code 
//      that is equivalent to the input file up to the use of new lines and other 
//      white space (THIS IS NOT A GREAT TEST (unless the filename of the generated 
//      code is made the same)).
//   4) Use multiple variable names in the same variable declaration (FIXED).
//
// Each of these are being addressed before moving this code into ROSE,
// merging it with the Wave support, and modifying the unparser to 
// use the token stream support.
*/

#include "rose.h"

// DQ (12/1/2013): Added switch to control testing mode for token unparsing.
// Test codes in the tests/roseTests/astTokenStreamTests directory turn on this 
// variable so that all regression tests can be processed to mix the unparsing of 
// the token stream with unparsing from the AST.
extern ROSE_DLL_API bool ROSE_tokenUnparsingTestingMode;

int
main ( int argc, char* argv[] )
   {
  // DQ (11/13/2014): Turn off the testing mode as a test of test2014_101.c and 
  // test2014_102.c which behave differently because they are different size ASTs).
  // DQ (12/1/2013): Set the ROSE token unparsing testing mode...
  // ROSE_tokenUnparsingTestingMode = true;
  // ROSE_tokenUnparsingTestingMode = false;
     ROSE_tokenUnparsingTestingMode = true;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // DQ (10/27/2013): We have to call the unparser since token stream unparsing is not an option in ROSE.
     return backend(project);
   }
