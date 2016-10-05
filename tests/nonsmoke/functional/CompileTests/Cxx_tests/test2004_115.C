// This testcode demonstrates the use of qualified type names within function parameter lists.

namespace myNamespace
   {
     class Domain {};
   }

void foo (myNamespace::Domain X);
void foo (myNamespace::Domain & X);

class myClass
   {
     public:
          class Domain {};
   };

void foo (myClass::Domain X);
void foo (myClass::Domain & X);

class Domain {};

void foo (Domain X);
void foo (Domain & X);

