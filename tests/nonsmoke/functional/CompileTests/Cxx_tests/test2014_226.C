// This is a similar example to test2013_303.C which shows how
// functions can be overloaded based only on template parameters.
// This example shows how similar classes can be defined, but 
// since overloading of classes is not legal, the classes must
// have different names.

class foo
   {
     public:
          void bar_function();
   };

// This is the template class for member functions.
template <void (foo::*M)()> class A 
   {
     public:
          A() {};         
   };

int main()
   {
  // Unparses as: "A< &bar_function > ();" when specified as: "A<&foo::bar_function>();" (which is an SgConstructorInitializer).
  // If specified as: "A<&foo::bar_function> a();" then it will work well (since it is a function declaration).
     A<&foo::bar_function>();
   }

// Template Member function must be unparsed with header "template < void (::foo::* M) () >"
// Bug: unparsed as: "template < void (::foo::*)() M >"


