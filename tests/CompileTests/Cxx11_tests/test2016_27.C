
namespace std 
   {
     template<typename _Alloc, typename _Tp>
     class __alloctr_rebind_helper
        {
          template<typename _Tp, typename... _Args>
          struct __construct_helper {};

          template<typename _Tp, typename... _Args>
          using __has_construct = typename __construct_helper<_Tp, _Args...>::type;

          template<typename _Tp, typename... _Args> 
          static __has_construct<_Tp, _Args...> _S_construct(_Tp* __p, _Args&&... __args);
        };
   }
