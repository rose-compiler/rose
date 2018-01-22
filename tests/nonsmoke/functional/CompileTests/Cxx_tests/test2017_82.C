#if 0
Dan,

Some ECP proxy apps (AMG 2013 and QuickSilver) use macros within OpenMP pragmas.
Right now somehow we do not see expanded macros within the pragmas in AST and the current OpenMP pragma parser cannot parse them properly.

This could be some limitation of the preprocessor used by EDG.
For GCC, its preprocessor can expand macros within OpenMP pragmas if -fopenmp is turned on.

For example:

#endif

//-------------- input code -----------------------------
 // cat define-directive.c
void foo(int a[10][10])
{
  int i;
  int j;

#define BIG 1
  int k = BIG ;
#define MY_SMP_PRIVATE i,j
#pragma omp parallel private (MY_SMP_PRIVATE)
#pragma omp for
  for (j =0; j< 10; j++)
    for (i =0; i< 10; i++)
      a[i][j] = 0;
}


#if 0
//---------------GCC preprocessed code by default -----------------------------------
gcc -E define-directive.c
# 1 "define-directive.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "define-directive.c"
void foo(int a[10][10])
{
  int i;
  int j;


  int k = 1 ;

#pragma omp parallel private (MY_SMP_PRIVATE) // <<-----------macros within pragmas not expanded by default
#pragma omp for
  for (j =0; j< 10; j++)
    for (i =0; i< 10; i++)
      a[i][j] = 0;
}

//---------------GCC preprocessed code with -fopenmp -----------------------------------
gcc -E -fopenmp define-directive.c

# 1 "define-directive.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "define-directive.c"
void foo(int a[10][10])
{
  int i;
  int j;


  int k = 1 ;


# 9 "define-directive.c"
#pragma omp parallel private (i,j)  // <<-----------macro expanded within pragma here
# 9 "define-directive.c"


# 10 "define-directive.c"
#pragma omp for
# 10 "define-directive.c" 
#endif
