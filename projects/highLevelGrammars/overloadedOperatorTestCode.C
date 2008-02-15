
#if 0
// This simple class declaration allows us to write transformations that are simple (fast) to compile
class InternalIndex
   {
     public:
          InternalIndex operator= ( const InternalIndex & rhs );

          friend InternalIndex operator+ ( const InternalIndex & lhs, int x );
   };
#endif

#include "simpleA++.h"

int
main()
   {
  // InternalIndex I,J;
     Index I,J;
     int n;

     doubleArray A,B,C;
     A(I+n+1) = B(I-(n/2)) + B(I-1) + B(I+1,J); // B(I) + C(I);

     return 0;
   }

