template <typename T>
class X
   {
     public:
          X();
         ~X();
          void foo();
          X operator+();
   };

X<int>::~X()
   {
   }

X<int>::X()
   {
   }

void X<int>::foo()
   {
   }

// DQ (9/5/2005): Commented out to allow tests with AST testing in place. It used to pass just fine!
// Commented out to permit passing with AST Tests! (used to pass)
X<int> X<int>::operator+( )
   {
     X<int> x;
     return x;
   }


