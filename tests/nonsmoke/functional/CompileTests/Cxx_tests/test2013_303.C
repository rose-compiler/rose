// This is a modification of the test code with was a bug report by Peter.
// Bug reported by Peter Collingbourne

class foo
   {
     public:
          int bar_variable;
          void bar_function();
   };

// This is the template function for member functions.
template <void (foo::*M)()> void test() {}

// This is the template function for member data.
template <int foo::*M> void test() {}

// Example of similar variable declarations used as a function parameter.
void foobar(void (foo::*X)(),int foo::*Y);

int main()
   {
#if 1
  // This is a variable declaration for a member function of class foo.
     void (foo::*X)() = &foo::bar_function;

  // This is a variable declaration for a data member of class foo.
     int foo::*Y = &foo::bar_variable;
#endif

#if 1
  // Unparses as: "test < &::foo::bar_variable > ();"
     test<&foo::bar_variable>();

  // Unparses as: "test < &foo::bar_function > ();" 
  // (likely this should have had the global qualifier "::", 
  // same as the variable data member case)
     test<&foo::bar_function>();
#endif
   }


