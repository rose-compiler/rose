template<class _Sp, class _Tp>
struct __traitor
   {
  // enum { __value = bool(_Sp::__value) || bool(_Tp::__value) };
  // enum { __value = bool(_Sp::__value) };
     enum { __value = bool(_Sp::__value) || bool(_Tp::__value) };
  // typedef typename __truth_type<__value>::__type __type;
   };
