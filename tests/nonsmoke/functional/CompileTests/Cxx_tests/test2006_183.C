
void foo();

typedef int Integer;

namespace A
   {
     void foo();
     typedef long Integer;
  // extern Integer a;
   }

void foobar()
   {
#if 0
  // Example of function call without global qualification (optional)
     foo();

  // Example of function call with global qualification (optional)
     ::foo();

     A::foo();

     ::A::foo();
#endif

  // With this typedef the declaration for "a" is required to have it's type qualified (using global qualification)
  // typedef unsigned long Integer;

  // Global qualification only required if the typedef in this scope is used.
     ::Integer a;

  // The type here will be different depending on if the typedef hijacks the type beng used!.
     Integer b;

  // The type in this variable declaration is required to be qualified (but not using global qualification).
     A::Integer c;
   }

