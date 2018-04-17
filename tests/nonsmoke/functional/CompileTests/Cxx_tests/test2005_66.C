// This ia part of a bug submitted by Brian White regarding the name mangling of 
// overloaded operator() and operator[] which appears to not be implemented in ROSE.
// Also "delete" appears to be mangles as "new" which is another bug!

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

// #include<cstdio>
#include <cstddef>
#include <exception>

#if 0
namespace std 
   {
     class bad_alloc: public exception {};

  // Note that the presence of "throw" does not show up in the name mangling!
     struct nothrow_t {};
     extern const nothrow_t nothrow;  // indicator for allocation that does not throw exceptions

     typedef void (*new_handler)();
     new_handler set_new_handler(new_handler new_p) throw();
   }
#endif

template <typename T>
class templateFoo
   {
     public:
          templateFoo() {}
       // templateFoo(int x) {}
          templateFoo(T t) {}

       // templateFoo(foo) {}

          void foobar_start() {}
          int operator()(int x) { return 0; }
          int operator[](int x) { return 0; }
          unsigned int operator+=(unsigned int x) { return 0; }

          void foobar_new_and_delete() {}
       // void* operator new (size_t) throw(std::bad_alloc) { return 0; }
          void* operator new (size_t) throw() { return 0; }
          void operator delete (void*) {}
          void* operator new[] (size_t) { return 0; }
          void operator delete[] (void*) {}

          void foobar_new_and_delete_with_extra_parameter() {}
          void* operator new (size_t,void*) { return 0; }
          void operator delete (void*,void*) {}
          void* operator new[] (size_t,void*) { return 0; }
          void operator delete[] (void*,void*) {}

          template <typename S>
          void foobarTemplate(S s) {}

          void foobar_end() {}


   };


class foo
   {

     public:

          foo() {}
          foo(int) {}

          void foobar_start() {}
          int operator()(int x) { return 0; }
          int operator[](int x) { return 0; }
          unsigned int operator+=(unsigned int x) { return 0; }

          void foobar_new_and_delete() {}
       // void* operator new (size_t) throw(std::bad_alloc) { return 0; }
          void* operator new (size_t) throw() { return 0; }
          void operator delete (void*) {}
          void* operator new[] (size_t) { return 0; }
          void operator delete[] (void*) {}

          void foobar_new_and_delete_with_extra_parameter() {}
          void* operator new (size_t,void*) { return 0; }
          void operator delete (void*,void*) {}
          void* operator new[] (size_t,void*) { return 0; }
          void operator delete[] (void*,void*) {}

          template <typename T>
          void foobarTemplate(T t) {}

          void foobar_end() {}
   };

template <typename T>
class templateFoobar
   {
     public:
          templateFoobar() {}
          templateFoobar(int x) {}
          templateFoobar(T t) {}

          templateFoobar(foo f) {}
          operator foo() {}
   };


int 
main()
   {
     foo f;
     foo *fptr;

  // Note that gnu g++ does not use "__" in name mangling but ROSE does (I feel more comfortable with the "__" in place).
     f.foobar_start();
     f(1); // __cl
     f[1]; // __vc  (Dan: I think it should be __ix)
     f += 1;
     fptr = new foo;  // __nw
     delete fptr;     // __dl

  // EDG does not appear to distinguish "array_new" from "new" as it does for "array_delete" and "delete"
     fptr = new foo[10]; // __na
     delete[] fptr;      // __da

     f.foobarTemplate<float>(0.0);

     f.foobar_end();

     templateFoo<int> g;
     templateFoo<int> *gptr;

     g.foobar_start();
     g(1); // __cl
     g[1]; // __vc  (Dan: I think it should be __ix)
     g += 1;
     gptr = new templateFoo<int>;  // __nw
     delete gptr;     // __dl

  // EDG does not appear to distinguish "array_new" from "new" as it does for "array_delete" and "delete"
     gptr = new templateFoo<int>[10]; // __na
     delete[] gptr;      // __da

     g.foobarTemplate<float>(0.0);

     g.foobar_end();     

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

