/* DQ (2/3/2011): Bug report from iastate: blocksize_reparse/short_test.upc */

#include<upc.h>

shared [5] char buffer[10*THREADS];
shared [8] char bufferA[8*THREADS];

int main()
{
   return 0;
}

