// x1 and x2 may alias to each other. If no-aliasing assumed, the loop can be parallelized.
void foo(double* x, int jp, int begin, int end, double rh1)
{
  double * x1, *x2; 

  x1 = x;
  x2 = x1 +jp; 

  for (int i = begin; i< end; i++)
  {   
    x1[i] += rh1; 
    x2[i] -= rh1; 
  }   
}

