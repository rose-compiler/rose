
#if 0
// This is a C++14 example:
template<class T>
constexpr T pi = T(3.1415926535897932385);  // variable template
 
template<class T>
T circular_area(T r) // function template
   {
     return pi<T> * r * r; // pi<T> is a variable template instantiation
   }
#endif

