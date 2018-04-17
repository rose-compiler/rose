// This is a class version of test2012_132.C (which does the same test with template functions)
// which is also simplified example from test2005_137.C.

// Class containing member function not availabe to primative types
class A
   {
   };

// Template function
template <typename T>
class foo
   {
   };

// specialization for when "int" is used
template <>
class foo<int>
   {
   };

int main()
   {
     foo<int>();
     foo<A>();
   }


