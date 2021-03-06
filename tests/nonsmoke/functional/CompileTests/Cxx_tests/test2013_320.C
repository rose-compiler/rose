// This is a similar example to test2013_303.C which shows how
// functions can be overloaded based only on template parameters.
// This example shows how similar classes can be defined, but 
// since overloading of classes is not legal, the classes must
// have different names.

class foo
   {
     public:
          int bar_variable;
          void bar_function();
   };

// This is the template class for member functions.
template <void (foo::*M)()> class A 
   {
     public:
          A() {};         
   };

// This is the template class for member data.
template <int foo::*M> class B
   {
     public:
          B() {};         
   };

#if 1
int main()
   {
  // Unparses as: "B< &bar_variable > ();" when specified as: "B<&foo::bar_variable>();" (which is an SgConstructorInitializer).
  // If specified as: "B<&foo::bar_variable> b();" then it will work well (since it is a function declaration).
     B<&foo::bar_variable>();

  // Unparses as: "A< &bar_function > ();" when specified as: "A<&foo::bar_function>();" (which is an SgConstructorInitializer).
  // If specified as: "A<&foo::bar_function> a();" then it will work well (since it is a function declaration).
     A<&foo::bar_function>();
   }
#endif


