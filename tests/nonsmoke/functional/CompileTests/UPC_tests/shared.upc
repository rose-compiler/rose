/*
examples for shared and unshared, global and static data in UPC
Liao, 7/7/2008
*/
/* ---------- unshared data (TLD)--------------------------*/
/*Unshared global variables , with extern */
extern int quux;

/*unshared global variables: scalar, array, w or w/o initializer */
int counter;
int counter2 = 100;

double myarray[10];
double myarray2[5]={0.0, 1.1, 2.2,3.3,4.4};

  /*special case: private to shared */
shared[4] int * p2s_p1; 
shared[4] int * p2s_p2 = 0; 

/*-----------shared data (SSD)-------------------------------
  shared scalar, array, initializer 
*/
shared int global_counter; 

#if 1
// DQ (2/17/2011): Remove the initializaion since EDG reports it as an error with EDG 4.0.
shared int global_counter2;
#else
shared int global_counter2 = 2; 
#endif

/* shared arrays */
shared[5] double array[100*THREADS];

/* Berkeley UPC compiler does not yet fully implement this. See their bug 36
*/
#if 1
// DQ (2/17/2011): Remove the initializaion since EDG reports it as an error with EDG 4.0.
shared[5] double array2[10*THREADS];
#else
shared[5] double array2[10*THREADS]={1.1, 2.2};
#endif

/* shared pointers */
shared int* shared[10] s2s_p4; /*shared to shared */
shared[10] int* shared s2s_p44; /*shared to shared */
shared[5] int* shared[8] s2s_p444; /*shared to shared */
int *shared s2p_p3; /*shared to private */
int *shared[5] s2p_p33; /*shared to private */
int foo()
{
/* -------unshared static data -----------*/
  static int counter; /* static scalar */
  static int counter2 =0; /* static scalar with initializer */

  static double fooArray [2]; /* static array */
  static double fooArray2 [2] = {3.1, 1.3}; /* static array */

/* -------shared static data -----------*/
  static shared int scounter; /* static shared scalar */

#if 1
// DQ (2/17/2011): Remove the initializaion since EDG reports it as an error with EDG 4.0.
  static shared int scounter2; /* static shared scalar with initializer */
#else
  static shared int scounter2 =0; /* static shared scalar with initializer */
#endif

  /*static shared array */

  static shared int sfooArray3[5*THREADS];

#if 1
// DQ (2/17/2011): Remove the initializaion since EDG reports it as an error with EDG 4.0.
  static shared int sfooArray5[5*THREADS];
#else
  static shared int sfooArray5[5*THREADS] = {1,2,3,4,5}; 
#endif
  static shared int* p2s_static;
}
int main()
{ 
  int * p1;         /* a private pointer to a private variable */
  shared int *p2s_p2; /* a private pointer to a shared variable, most useful */
  shared[5] int *p2s_p22; /* a private pointer to a shared variable, most useful */
  return 0;
}
