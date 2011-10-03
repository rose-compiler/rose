/*
THREADS: number of threads
Used as array dimension 

June. 19, 2008
Liao
*/
shared int a[THREADS];
shared int b[5*THREADS];
shared int c[THREADS*5];
/*
int a[THREADS];
only arrays of a shared type can be dimensioned to a 
multiple of THREADS
*/


