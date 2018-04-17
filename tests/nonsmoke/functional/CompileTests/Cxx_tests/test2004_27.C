// This was a bug at one point which caused the same class declaration to be entered twice 
// into the global scope.  The bug is fixed and a test on the AST has not been introduced 
// which makes such that in any single scope, there are not redundent pointers to the same
// statements in the statement list of that single scope.  The test, and our design, still 
// allows for a single statement to be referenced from multiple locations, just not in the 
// single list of statemetns in each scope.  See ROSE/AstProcessingLib/AstTests.C for more 
// details.

class A;

class A 
   {
   };


