// This should have default (public) access.
// And it can't be marked a a friend since it is not declared in the context of a class.
void foobar();

class A
   {
  // Example of defining and nondefining declaration's access modifier being different.
  // For any non-friend defining declaration defined in the class, it's non-defining 
  // declaration's access must match the defining declaration.
     void foo(void) {}

     int x;

  // This should have default (public) access.
     friend void foobar() {}
   };


void foo_main()
   {
     foobar();
   }
