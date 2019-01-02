// Test if the same data member accesses from two different objects
// are treated as different memory location access.
//
// In AST, both varRefExps are pointing to the same variable symbol.
// This can be confusing to analysis.
// Liao 7/7/2016
#include <omp.h> 

struct VectorXY 
{
  double x;
  double y;
}
;
struct VectorXY v1[10];
struct VectorXY v2[10];

void applyVelocity()
{
  int in;
// We use pragma to indicate expected remaining dependence relations 
// which prevent auto parallelization
  
#pragma autopar remain_dep(4)
  
#pragma omp parallel for private (in)
  for (in = 0; in <= 9; in += 1) {
    v1[in] . y = v2[3] . y;
  }
}
