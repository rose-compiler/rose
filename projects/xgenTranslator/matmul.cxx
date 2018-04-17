#include "RAJA/RAJA.hxx"
#include <omp.h>
#include <cstdlib>

// The "switcher" policy lets us randomly select between OpenMP and Sequential
// execution for the inner loop of the matrix multiply.
typedef RAJA::switcher_exec policy_one;
// Alternatively, we can just run in serial:
// typedef RAJA::simd_exec policy_one;


// The policies for the forallN traversal methods are a lot more complicated,
// since each separate segment can have its own policy, and we can also apply
// some additional stuff like tiling. See the file
// raja/include/RAJA/forallN_generic.hxx for implementation details.
//
// TODO: We don't currently tune these, but we want to!
typedef RAJA::NestedPolicy<RAJA::ExecList<RAJA::seq_exec, RAJA::seq_exec> > policy_n;

// Reduction policy is always OpenMP, because we need to support threaded
// reductions if our policy switcher selects OpenMP.
// TODO: we would like to couple this to the switched policy
typedef RAJA::omp_reduce reduce_policy;


int main(int argc, char* argv[]) {
  const int n = 15;

  double* a = new double[n*n];
  double* b = new double[n*n];
  double* c = new double[n*n];

  RAJA::RangeSegment is(0,n);
  RAJA::RangeSegment js(0,n);
  RAJA::RangeSegment ks(0,n);


  RAJA::forallN<policy_n>(is, js, [=] (int i, int j) {
    a[i*n+j] = i + 0.1*j;
    b[i*n+j] = fabs(i-j);
    c[i*n+j] = 0.0;
  });

  RAJA::forallN<policy_n>(is, js, [=] (int i, int j) {

    RAJA::ReduceSum<reduce_policy, double> sum(c[i*n+j]) ;

    RAJA::forall<policy_one>(ks, [=] (int k) {
      sum += a[i*n+j]*b[k*n+j];
    });

    c[i*n+j] = double(sum);
  });

  printf("Element at c[3][3] is %f\n", c[3*n+3]);

  return 0;
}
