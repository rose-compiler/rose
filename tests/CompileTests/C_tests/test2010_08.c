// Liao, 11/9/2010
// A case to show function symbol's file information is totally 
// different from SgFunctionRefExp's location information
//
// As I improved the unparser to skip initializers from a different file, 
// accurate file info for SgFunctionRefExp is needed to unparse the 
// initializer list now. 
#include "test2010_08.h"
typedef int (*a_ptr_to_test_function)(void * logFile);
typedef struct
{
  char *name;
  a_ptr_to_test_function pass;
  a_ptr_to_test_function cross_pass;
}
testcall;

static testcall alltests[] = {
//#include "feasible_test.h"
 {"has_openmp",check_has_openmp,crosscheck_has_openmp},
// {"has_openmp",check_has_openmp}, must have three element to reproduce the error msg
  {"end", 0, 0}
};

