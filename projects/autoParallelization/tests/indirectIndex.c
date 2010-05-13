// A loop with array references using indirect indexing
//
// Conventional parallelization algorithms will not parallelize the loop
// since indirect indexing may result in overlapped elements being accessed,
// which in turn introduces loop carried dependencies. 
//
// However, if users can provide semantics that the indirect indexing will
// not result in overlapping elements (or unique elements), the loop can be parallelized.
//
// This is a simplified version based on code examples provided by Jeff Keasler.
//
//  Liao, 5/12/2009
#define length 100
double eps[length];
int zoneset[length];

void StressCheckEpsFail( double eps_failure_model)
{
  int i, index;
  for (i = 0 ; i < length ; i++) {
    index = zoneset[i] ;
    eps[index] = eps_failure_model * 1.01;
    eps[zoneset[i]] = 1.01;
  }
}

