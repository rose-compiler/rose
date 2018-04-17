// This test is similar to test2013_03.C but without the templates.
// It is being developed as a comparison to the bug demonstrated 
// as part of the copyAST problem for test2005_06.C (for which 
// test2013_03.C is the relevant portion demonstrating the bug).
// This is a result of changes on 1/14/2013 to make template 
// function instantiations be traversed in the AST (previously
// they were not added to the AST if the location where they
// were added was different from their scope).

// Making the template instantiations design in the AST match that
// of non template behavior is a specific goal.

// template <typename T>
class X
   {
     public:
#if 1
       // friend X<T> & operator+( X<T> & i, X<T> & j)
          friend X & operator+( X & i, X & j)
#else
          X<T> & operator+( X<T> & i)
#endif
             {
               return i;
             }
   };

int main()
   {
  // X<int> y,z;
     X y,z;

#if 1
     y + z;
#endif

     return 0;
   }

