/*
Hi Dan,

It seems that references to class member variables are (a) missing
an SgAddressOfOp and (b) unqualified with class name when unparsed.
See for example the attached code.  Is this a known problem?

Thanks,
-- Peter

*/


struct foo
   {
     int bar;
     int member_function( int x );
   };

int global_function( int x );

namespace X
   {
     int namespace_function( int x );
   }


// This was a bug in test2007_01.C
// template <int foo::*V>
// class test {};
// typedef test<&foo::bar> tt;

typedef int (foo::*PMF)(int);
typedef int (*PF)(int);

// Declaration of pointers to name space specific functions is not allowed!
// typedef int (X::*PNF)(int);
typedef int (*PNF)(int);

int main()
   {
  // This was reported in test2007_01.C but is isolated into a separate test code (address operator is dropped or never generated).
     int foo::*v = &foo::bar;

// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ >= 3)
     PMF member_function_pointer; // = foo::member_function;
#else
     PMF member_function_pointer = foo::member_function;
#endif

     PF global_function_pointer = global_function;

     PNF namespace_function_pointer = X::namespace_function;

#if 1
  // Build a struct object of type "foo"
     foo X;

  // Using "v": Initialize the bar data member of X
     X.*v = 42;

  // Build a pointer to an object of type "foo"
     foo* Xptr;

  // Using "v": Access through a pointer of type "foo"
     Xptr->*v = 42;
#endif

     return 0;
   }
