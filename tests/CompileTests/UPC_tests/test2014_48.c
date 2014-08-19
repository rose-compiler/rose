
// int a;
// int* b;
// const int * c;
// int const * d1;
// int * const d2;

// shared int * e;

// Although "int shared *" and "int * shared" are type equivalent
// this does not generate an error to EDG's UPC support.
int shared * f[5];

// This is an error in UPC: "one dimension of an array of a shared type must be a multiple of THREADS when the number of threads is nonconstant"
// To eliminate the error we have to specify the UPC THREADS variable using the option: "-rose:upc_threads 1"
// int * shared f2[5];

#if 0
typedef struct 
   {
  /* SHARED double * SHARED * SHARED * EOutAE; */
     shared double * g;
   } AngEng;
#endif
