// This is a simplified example from test2005_137.C

// Class containing member function not availabe to primative types
class A
   {
     public:
         int increment();
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

int main()
   {
     foo<int>();
     foo<A>();
   }


