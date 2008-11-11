// Test code to demonstration of dwarf support.
// Designed to be small because Dwarf is verbose.

namespace example_namespace
   {
     int a;
   };


int main()
   {
     int x = 42;

  // Loops are not recognised in Dwarf...
     for (int i = 0; i < 10; i++)
        {
          x = (x % 2) ? x * 2 : x + 1;
        }

     return 0;
   }

