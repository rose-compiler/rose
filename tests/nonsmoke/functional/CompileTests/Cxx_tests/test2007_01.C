/*
Hi Dan,

It seems that references to class member variables are (a) missing
an SgAddressOfOp and (b) unqualified with class name when unparsed.
See for example the attached code.  Is this a known problem?

Thanks,
-- Peter

*/

struct foo { int bar; };

template <int foo::*V>
class test {};

typedef test<&foo::bar> tt;

// This part of the test is isolated into test2007_04.C
int main()
   {
     int foo::*v = &foo::bar;
   }
