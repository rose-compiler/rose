template<typename _Res, typename _Class>
class _Mem_fn<_Res _Class::*>
   {
  // Handle smart pointers and derived
     template<typename _Tp, typename _Req = _Require<_NotSame<_Class*, _Tp>>>
	  auto
	  operator()(_Tp&& __unknown) const noexcept(noexcept(std::declval<_Mem_fn*>()->_M_call(std::forward<_Tp>(__unknown), &__unknown)))
        -> decltype(this->_M_call(std::forward<_Tp>(__unknown), &__unknown))
	     { return _M_call(std::forward<_Tp>(__unknown), &__unknown); }
  };


