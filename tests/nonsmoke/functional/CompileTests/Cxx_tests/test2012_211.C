template<typename _Tp>
struct __is_pointer
   {
   };

template<typename _Tp>
struct __is_pointer<_Tp*>
   {
   };

