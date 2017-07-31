// This is a C++14 example:
// template<typename _MemberPointer> class _Mem_fn;
  
// template<class T> constexpr T pi = T(3.1415926535897932385);  // variable template
template<class T> constexpr T pointer = T(0L);  // variable template

template<typename _Res, typename _Class, typename... _ArgTypes>
void foobar() // function template
   {
     pointer<_Res (_Class::*)(_ArgTypes...)>; // pointer<T> is a variable template instantiation
   }

