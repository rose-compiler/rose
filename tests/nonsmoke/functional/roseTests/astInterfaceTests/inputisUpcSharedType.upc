/* shared scalar */
shared int global_counter; 
int global_counter2;

/* shared arrays */
shared[5] double array[100*THREADS];
double array2[100];

/* shared pointers */
shared[5] int* shared[8] s2s_p444; /*shared to shared */

int * p1;

int *shared s2p_p3; /*shared to private */

shared int * p2s_p11; /*private to shared */

/* typedef  */
typedef shared int mytype1;
mytype1 s_1;
