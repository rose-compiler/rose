//    This example demonstrates why we can't enforce the all member functions
// have only one non-defining declarations.  Basically because the template instantiations
// don't follow the same rules (or not as they might be generally stated).
// We can talk about if this is important to enforce, or if template instantations 
// should be handled as a special case (see test2005_60.C for an example of
// a case using non-template member functions).

// As a result of this, some declaration will be built and never referenced
// since they can't be reused!!!  Not sure how to do this better!

// Example of multiple declarations (one of which contains default arguments)
void foo( int x, int y);
void foo( int x, int y);
void foo( int x, int y = 0);
void foo( int x, int y);

class A
   {
     void foo();
   };

// Normal non-template member functions only allow one non-defining member function declaration
// This is NOT allowed (redeclaration of member function)
// void A::foo();

template <typename T>
class B
   {
     void foo();
   };

// This is now to generate two non-defining member function declarations
// This IS allowed, and is a forward declaration of the specialization of the member function
template<>
void B<int>::foo();


void foo()
   {
  // Test for loop with multiple variables in the declaration (has nothing to do with test above!)
     for (int i=0,j,k=1,l,m=2,n; i < 10; i++);
   }
