// *********************
// Function form of test
// *********************

// Class containing member function not availabe to primative types
class A
   {
     public:
          int increment() { return 0; }
   };

// Template function
template <typename T>
int foo()
   {
  // This would be an error if T was a primative type
     T t;
     return t.increment();
   }

// specialization for when "int" is used
template <>
int foo<int>()
   {
     return 0;
   }

// general template instatiation directive
// This will trigger an error if the specialization has not been put out!
template int foo<A>();

// template instatiation directives for specialization
// It seems that it is an error to use a directive on a specialization :-)
// template int foo<int>();

int main()
   {
     foo<int>();
     foo<A>();
   }


