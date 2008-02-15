// Test code for overloaded member functions of a templated class where at least 
// one is a template function (as opposed to a non-template member fucnction).

// this is a class template
template <typename T>
class X
   {
     public:
       // This is an overloaded non-template member function
          void foobar(T t){};
#if 0
       // This is an overloaded non-template member function
          void foobar(T t, int i ){};
#endif
#if 1
       // This is an overloaded template member function
          template <typename S>
          void foobar(S t){};
#endif
   };



// For gnu 4.1.2, if we generate the member function template specialization
// then we have to generate the class template specialization as well.
// It appears that the only exception is for constructors, I think.
#if 0
// This is required to be output for this to be compiled with 4.1.2
// if the prototype for foobar and it's defnition are output.  However,
// the prototype for foobar and it's definition should not be output!
// This is only done as part of the AST copy mechanism, and it is a bug.

template <>
class X <int>
   {
     public :
          void foobar ( int t );
          template < typename S >
          void foobar ( S t ) { };
   };
#endif

void foo()
   {
     X<int> a;

     a.foobar(1);
  // a.foobar(1,2);
     a.foobar(3.14159265);
   }


// Make this compile and link
int main()
   {
     return 0;
   }

