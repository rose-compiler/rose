/*
 * A test translator to process OpenMP Accelerator input
 *
 * by Liao, 8/11/2009
*/
#include "rose.h"

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

//TODO: turn this back on once blockDim.x * blockIdx.x + threadIdx.x is built properly in omp_lowering.cpp
//  AstTests::runAllTests(project);
  return backend(project);
}

