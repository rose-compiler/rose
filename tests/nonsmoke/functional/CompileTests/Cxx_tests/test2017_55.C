
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

#if 0
// This is C++11 specific
template<typename _MemberPointer>
class _Mem_fn;
  
// template<typename _Res, typename _Class, typename... _ArgTypes>
// class _Mem_fn<_Res (_Class::*)(_ArgTypes...)>

// This is the templated typedef:
template<typename _Res, typename _Class, typename... _ArgTypes>
using X = _Mem_fn<_Res (_Class::*)(_ArgTypes...)>;
#endif

