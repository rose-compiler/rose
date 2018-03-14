// parallel + for + simd
void foo(int ni, int nj, int nk, double E[128][128], double A[128][128], double B[128][128])
{
  int c5;
  int c10;
  int c2;
  int c1;
  int c6;
  int c7;

#pragma omp parallel for private(c7, c6, c2, c10, c5)
  for (c1 = 0; c1 <= ni; c1++) {
    for (c2 = 0; c2 <= nj; c2++) {
      for (c5 = 0; c5 <= nk ; c5++) {
        for (c6 = 16 * c5; c6 <= nk; c6++) {
          for (c7 = 16 * c2; c7 <= nj; c7++) {
#pragma omp simd 
            for (c10 = 16 * c1; c10 <= ni; c10++) {
              E[c10][c7] += A[c10][c6] * B[c6][c7];
            }    
          }    
        }
      }
    }    
  } 
} // end foo()
