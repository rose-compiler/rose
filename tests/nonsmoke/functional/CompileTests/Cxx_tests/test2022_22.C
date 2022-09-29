// Bug specific to EDG 6.3

template<typename _MemFunPtr, bool __is_mem_fn = false>
class _Mem_fn_base
   {
   };

template<typename _MemObjPtr>
class _Mem_fn_base<_MemObjPtr, false>
   {
   };

template<typename _MemberPointer>
struct _Mem_fn; // undefined

template<typename _Res, typename _Class>
struct _Mem_fn<_Res _Class::*>
   : _Mem_fn_base<_Res _Class::*>
   {
     using _Mem_fn_base<_Res _Class::*>::_Mem_fn_base;
   };

