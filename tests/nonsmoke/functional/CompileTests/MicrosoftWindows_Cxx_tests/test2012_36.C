// This testcode demonstrates the use of qualified type names within function parameter lists.

namespace myNamespace
   {
     class Domain {};
   }

void foo1a (myNamespace::Domain X);
// void foo1b (myNamespace::Domain X);
// void foo (myNamespace::Domain & X);

class myClass
   {
     public:
          class Domain {};
   };

// void foo ();
// void foo (myNamespace::Domain & X);
void foo2a (myClass::Domain X);
// void foo2b (myClass::Domain X);
// void foo3 (myNamespace::Domain X);

#if 1
void foo (myClass::Domain & X);

class Domain {};

void foo (Domain X);
void foo (Domain & X);
#endif
