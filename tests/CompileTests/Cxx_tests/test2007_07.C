// Test use of "main" as a function name, in different locations.
// we want to answer the question of what the qualificat name for 
// main should be ("main" or "::main").

namespace X
   {
     int main();
   }

class Y
   {
     int main();
   };

int main()
   {
     return 0;
   }

