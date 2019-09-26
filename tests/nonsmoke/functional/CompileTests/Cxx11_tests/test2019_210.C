
template <class T>
struct A_
   {
     int f();
   };

template<> struct A_<int>
   {
     int f();
   };

// Original code: int A_<int>::f()
// Unparse as:    template<> int A_< int > ::f()
int A_<int>::f()
   {
     return 42;
   }

