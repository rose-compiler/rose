#define DEMONSTRATE_BUG 1

class X
   {
     public:
#if DEMONSTRATE_BUG
          friend void transpose();
#endif
   };

void transpose()
   {
   }

