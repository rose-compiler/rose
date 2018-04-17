#include <stdio.h>

typedef struct _int4 {
   int s0, s1, s2, s3;
} int4;

int main(int argc, char * argv[])
{
   int4 v;

   v = (int4) {0, 1, 2, 3};   

   printf("v = (%d,%d,%d,%d)\n", v.s0, v.s1, v.s2, v.s3);

   return 0;
}
