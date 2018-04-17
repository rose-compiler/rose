// Another positive test case:  with const double for rhs 
void foo_cd(double* x, int jp, int kp, int begin, int end, const double rh1)
{
   double * x1, *x2, *x3, *x4; 
   
   x1 = x;
   x2 = x +1;
   x3 = x1 + jp;
   x4 = x1 + kp;
   
   for (int i = begin; i< end; i++)
   {
      x1[i] += rh1; 
      x2[i] -= rh1;
      x3[i] *= rh1; 
      x4[i] /= rh1;
   }
}

