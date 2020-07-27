
struct a 
   {
     a(int i, int j, ...);
   };

struct b : a 
   {
     using a::a;
   };

void foobar()
   {
  // This fails for gnu g++ 6.1 (but works properly within ROSE).
     b bc (4,5,6);
   }


