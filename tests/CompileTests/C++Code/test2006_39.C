
using namespace std;

void foo(int x)
   {
  // int x; note that x is already in the scope from the function parameter list
     int y;

  // Are these the same?
     for (;;) break;

     for (;;) { break; }
   }


