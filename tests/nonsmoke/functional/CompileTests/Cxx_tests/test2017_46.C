
template<typename _Res>
class _Mem_fn;

// template<typename _Res, int _Class>
// class _Mem_fn<_Res[_Class]>
template<typename _Res, typename _Class, typename... _ArgTypes>
class _Mem_fn<_Res (_Class::*)(_ArgTypes...)>
   {
     int x;
   };
