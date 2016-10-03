/* get_data.c -- Parallel Trapezoidal Rule, uses basic Get_data function for
*     input.
*
* Input:
*    a, b: limits of integration.
*    n: number of trapezoids.
* Output:  Estimate of the integral from a to b of f(x)
*    using the trapezoidal rule and n trapezoids.
*
* Notes: *    1.  f(x) is hardwired.
*    2.  Assumes number of processes (p) evenly divides
*        number of trapezoids (n).
*
* See Chap. 4, pp. 60 & ff in PPMPI.
*/
#include <stdio.h>

/* We'll be using MPI routines, definitions, etc. */
#include "mpi.h"

main(int argc, char** argv) {
 int         my_rank;   /* My process rank           */
 int         p;         /* The number of processes   */
 float       a;         /* Left endpoint             */
 float       b;         /* Right endpoint            */
 int         n;         /* Number of trapezoids      */
 float       h;         /* Trapezoid base length     */
 float       local_a;   /* Left endpoint my process  */
 float       local_b;   /* Right endpoint my process */
 int         local_n;   /* Number of trapezoids for  */
                        /* my calculation            */
 float       integral;  /* Integral over my interval */
 float       total;     /* Total integral            */
 int         source;    /* Process sending integral  */
 int         dest = 0;  /* All messages go to 0      */
 int         tag = 0;
 MPI_Status  status;

 void Get_data(float* a_ptr, float* b_ptr,
      int* n_ptr, int my_rank, int p);
 float Trap(float local_a, float local_b, int local_n,
           float h);    /* Calculate local integral  */

 /* Let the system do what it needs to start up MPI */
 MPI_Init(&argc, &argv);

 /* Get my process rank */
 MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

 /* Find out how many processes are being used */
 MPI_Comm_size(MPI_COMM_WORLD, &p);

 Get_data(&a, &b, &n, my_rank, p);

 h = (b-a)/n;    /* h is the same for all processes */
 local_n = n/p;  /* So is the number of trapezoids */

 /* Length of each process' interval of
  * integration = local_n*h.  So my interval
  * starts at: */
 local_a = a + my_rank*local_n*h;
 local_b = local_a + local_n*h;
 integral = Trap(local_a, local_b, local_n, h);

 /* Add up the integrals calculated by each process */
 if (my_rank == 0) {
     total = integral;
     for (source = 1; source < p; source++) {
         MPI_Recv(&integral, 1, MPI_FLOAT, source, tag,
             MPI_COMM_WORLD, &status);
         total = total + integral;
     }
 } else {         MPI_Send(&integral, 1, MPI_FLOAT, dest,
         tag, MPI_COMM_WORLD);
 }

 /* Print the result */
 if (my_rank == 0) {
     printf("With n = %d trapezoids, our estimate\n",
         n);
     printf("of the integral from %f to %f = %f\n",
         a, b, total);
 }

 /* Shut down MPI */
 MPI_Finalize();
} /*  main  */


/********************************************************************/
/* Function Get_data
* Reads in the user input a, b, and n.
* Input parameters:
*     1.  int my_rank:  rank of current process.
*     2.  int p:  number of processes.
* Output parameters: *     1.  float* a_ptr:  pointer to left endpoint a.
*     2.  float* b_ptr:  pointer to right endpoint b.
*     3.  int* n_ptr:  pointer to number of trapezoids.
* Algorithm:
*     1.  Process 0 prompts user for input and
*         reads in the values.
*     2.  Process 0 sends input values to other
*         processes.
*/
void Get_data(
      float*  a_ptr    /* out */,
      float*  b_ptr    /* out */,
      int*    n_ptr    /* out */,
      int     my_rank  /* in  */,
      int     p        /* in  */) {

 int source = 0;    /* All local variables used by */
 int dest;          /* MPI_Send and MPI_Recv       */
 int tag;
 MPI_Status status;

 if (my_rank == 0){
     printf("Enter a, b, and n\n");
     scanf("%f %f %d", a_ptr, b_ptr, n_ptr);
     for (dest = 1; dest < p; dest++){
         tag = 0;
         MPI_Send(a_ptr, 1, MPI_FLOAT, dest, tag,
             MPI_COMM_WORLD);
         tag = 1;
         MPI_Send(b_ptr, 1, MPI_FLOAT, dest, tag,
             MPI_COMM_WORLD);
         tag = 2;
         MPI_Send(n_ptr, 1, MPI_INT, dest, tag,
             MPI_COMM_WORLD);
     }
 } else {
     tag = 0;
     MPI_Recv(a_ptr, 1, MPI_FLOAT, source, tag,
         MPI_COMM_WORLD, &status);
     tag = 1;
     MPI_Recv(b_ptr, 1, MPI_FLOAT, source, tag,
         MPI_COMM_WORLD, &status);
     tag = 2;
     MPI_Recv(n_ptr, 1, MPI_INT, source, tag,
             MPI_COMM_WORLD, &status);
 }
} /* Get_data */


/********************************************************************/
float Trap(
       float  local_a   /* in */,
       float  local_b   /* in */,
       int    local_n   /* in */,
       float  h         /* in */) {

 float integral;   /* Store result in integral  */
 float x;
 int i;

 float f(float x); /* function we're integrating */

 integral = (f(local_a) + f(local_b))/2.0;
 x = local_a;
 for (i = 1; i <= local_n-1; i++) {
     x = x + h;
     integral = integral + f(x);
 }
 integral = integral*h;
 return integral;
} /*  Trap  */


/********************************************************************/
float f(float x) {
 float return_val;
 /* Calculate f(x). */
 /* Store calculation in return_val. */
 return_val = x*x;
 return return_val;
} /* f */
