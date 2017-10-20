

// template<typename _Res> void foobar (_Res);
// template<typename _Res, typename _Class> void foobar(void (_Class::*)(_Res));


template<typename _MemberPointer>
class _Mem_fn;
  
// template<typename _Res, typename _Class, typename... _ArgTypes>
// class _Mem_fn<_Res (_Class::*)(_ArgTypes...)>

// This is the templated typedef:
template<typename _Res, typename _Class, typename... _ArgTypes>
using X = _Mem_fn<_Res (_Class::*)(_ArgTypes...)>;

