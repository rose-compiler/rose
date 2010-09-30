// This test code includes the Non-standard Berkeley UPC extensions as a test of ROSE
// processing the header file. We would like to get test code that would test the us
// these Berkeley UPC extensions.
#include<bupc_extensions.h>

int main()
   {
     printf("Hello World from thread %d of %d threads\n",\
	  MYTHREAD,THREADS);
     upc_barrier;
     return 0;
   }
