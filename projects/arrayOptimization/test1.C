//#include "A++.h"
#include "simpleA++.h"

int main()
{
   floatArray A(10);
   floatArray B(10);
   floatArray C(10);

   Index I(0,5,1);

   C(0) = C(9);
   C = A;
   B(I) = C(I+1);
   B = B + C;
   A = B + C + A;
   A(I-1) = (B(I+1) + C(I-1)) / 2;


   return 0;
}
