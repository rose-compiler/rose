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

int main()
   {
  // Unparses as: "test < &::foo::bar_variable > ();"
     test<&foo::bar_variable>();

  // Unparses as: "test < &foo::bar_function > ();" 
  // (likely this should have had the global qualifier "::", 
  // same as the variable data member case)
     test<&foo::bar_function>();
   }
