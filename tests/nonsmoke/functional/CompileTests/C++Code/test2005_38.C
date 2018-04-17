// This test attempts to demonstrate the use of the address of "&" operator
// but the EDG translation generates pointer arithmetic instead.  So I have
// not yet demonstrated the bug that I was trying to demonstrate (or any use of "&").

void foobar ( double *d1, double *d2 );

void foo()
   {
     double **array;
     int n;
     array[n] = new double[100];
     foobar(&(array[n][n]),&array[n++][n]);
   }
