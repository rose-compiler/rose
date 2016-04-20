
#include "globalVariablesInLambdas_test.h"

int otherGlobalVar = 0;
static int gvar1 = 0, gvar2 = 0, gvar3 = 0, gvar4 = 0;

void foo()
{
   for_all([=](int i) {
      int val = gvar1 + gvar2;
      int other_val = gvar1 * gvar2;
      // do other stuff
   });

   for_all([=](int i) {
      int val = gvar1 + gvar4;
      int other_val = gvar1 * gvar2;
      // do other stuff
   });

   int val = otherGlobalVar;
}


void bar()
{
   int val = otherGlobalVar;

   for_all([=](int i) {
      int val = gvar2 + gvar2;
      int other_val = gvar2 * gvar2;
      // do other stuff
      other_val = 2 * other_val;
   });

   val++;
   val--;

   for_all([=](int i) {
      int val = gvar4 + gvar4;
      int other_val = gvar3 * gvar4;
      // do other stuff
   });
}

// Suggested output for case of onlyInLambdaFunctions == false: 
// The output would be something like:
// file 'xyz.cc': function 'foo': gvar1, gvar2, gvar4
// file 'xyz.cc': function 'bar': gvar2, gvar3, gvar4

// Actual output of test run (onlyInLambdaFunctions == false):
// file: /home/dquinlan/ROSE/git_rose_development/tests/CompileTests/Cxx11_tests/globalVariablesInLambdas_test_01.C function: foo global variables: otherGlobalVar gvar1 gvar2 gvar4 
// file: /home/dquinlan/ROSE/git_rose_development/tests/CompileTests/Cxx11_tests/globalVariablesInLambdas_test_01.C function: bar global variables: otherGlobalVar gvar2 gvar3 gvar4 

// Actual output of test run (onlyInLambdaFunctions == true):
// file: /home/dquinlan/ROSE/git_rose_development/tests/CompileTests/Cxx11_tests/globalVariablesInLambdas_test_01.C function: foo global variables: gvar1 gvar2 gvar4 
// file: /home/dquinlan/ROSE/git_rose_development/tests/CompileTests/Cxx11_tests/globalVariablesInLambdas_test_01.C function: bar global variables: gvar2 gvar3 gvar4 
