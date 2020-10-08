#include <stdlib.h>

template <typename T>
class A
   {
     public:
          A();
          A( int *x, int y);
          int & operator[](int i);
          A *operator->() const { return Aptr; }
          A& operator*() const  { return *Aptr; }
          const A* operator&() const  { return this; }
          A ( const A & X ) {}

          A*  Aptr;
          void* operator new(size_t s);
          void* operator new(size_t s, const A & x);
   };

class B : public A<int>
   {
   };

class C : public A<A<int> >
   {
   };

void fooError( A<int> & a)
   {
      A<int> *a_pointer;

      a_pointer = new A<int> ((A<int>&)a);
   }

void fooError( B & b)
   {
      A<int> *a_pointer;

      a_pointer = new A<int> ((A<int>&)b);
   }

void fooError( C & c)
   {
      A<A<int> > *a_pointer;

      a_pointer = new A<A<int> > ((A<A<int> >&)c);
   }

