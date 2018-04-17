/*
Hi Dan,

It seems that references to class member variables are (a) missing
an SgAddressOfOp and (b) unqualified with class name when unparsed.
See for example the attached code.  Is this a known problem?

Thanks,
-- Peter

*/


struct foo { int bar; };

// This was a bug in test2007_01.C
// template <int foo::*V>
// class test {};
// typedef test<&foo::bar> tt;

int main()
   {
  // This was reported in test2007_01.C but is isolated into a separate test code (address operator is dropped or never generated).
     int foo::*v = &foo::bar;

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
