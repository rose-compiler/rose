template<typename _MemberPointer>
class _Mem_fn;

template<typename _Res, typename _Class>
class _Mem_fn<_Res _Class::*> {
    _Mem_fn(_Res _Class::*__pm) noexcept : __pm(__pm) { }

    _Res _Class::*__pm;
};
