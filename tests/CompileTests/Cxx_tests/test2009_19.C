#if 0
Hi Dan,

This function definition is legal in C++ (note middle formal argument), but crashes the ROSE unparser:

void f(int length, double *, double *x)
{
   for (int i=0; i<length; ++i)
      x[i] = -x[i] ;
}

Several function like this are in ALE3D.

-Jeff
#endif

void f(int length, double *, double *x)
   {
     for (int i=0; i<length; ++i)
          x[i] = -x[i] ;
   }

