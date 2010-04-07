
template <typename T>
class X
   {
     public:
          X();
         ~X();
          void foo();
          X operator+();
   };

// DQ (2/20/2010): Newer versions of GNU g++ require "template<>"
template<> X<int>::~X()
   {
   }

// DQ (2/20/2010): Newer versions of GNU g++ require "template<>"
template<> X<int>::X()
   {
   }

// DQ (2/20/2010): Newer versions of GNU g++ require "template<>"
template<> void X<int>::foo()
   {
   }

// DQ (2/20/2010): Newer versions of GNU g++ require "template<>"
// DQ (9/5/2005): Commented out to allow tests with AST testing in place. It used to pass just fine!
// Commented out to permit passing with AST Tests! (used to pass)
template<> X<int> X<int>::operator+( )
   {
     X<int> x;
     return x;
   }


