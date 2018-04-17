
extern int x;

void foo()
   {
     int y;
  // This demonstrates references to variable that are in the current 
  // scope and outside of the current scope (but in a parent scope).
     y = x;
  // foo2();
   }
