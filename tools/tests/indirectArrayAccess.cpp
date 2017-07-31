// Another positive test case:  indirect array access
void foo_cd(double* x, int * indexSet, int jp, int kp, int begin, int end, const double rh1)
{
   double * x1, *x2, *x3, *x4; 
   
   x1 = x;
   x2 = x +1;
   x3 = x1 + jp;
   x4 = x1 + kp;
   
   for (int idx = begin; idx< end; idx++)
   { 
      // i is derived from loop idx, via an array
      const int i = indexSet[idx];
      x1[i] += rh1; 
      x2[i] -= rh1;
      x3[i] *= rh1; 
      x4[i] /= rh1;
   }
}

