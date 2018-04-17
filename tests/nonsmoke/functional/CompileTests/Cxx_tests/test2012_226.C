void foobar()
   {
  // bool x = (true && true);
     bool x = (1 < 2);
     if (x) {}
   }

template <typename _BI1, typename _BI2>
inline _BI2
copy_backward(_BI1 __first, _BI1 __last, _BI2 __result)
   {
  // concept requirements
  // __glibcxx_function_requires(_BidirectionalIteratorConcept<_BI1>)
  // __glibcxx_function_requires(_Mutable_BidirectionalIteratorConcept<_BI2>)
  // __glibcxx_function_requires(_ConvertibleConcept<
  // typename iterator_traits<_BI1>::value_type,
  // typename iterator_traits<_BI2>::value_type>)
  // __glibcxx_requires_valid_range(__first, __last);

  // const bool __bi1 = __is_normal_iterator<_BI1>::__value;
  // const bool __bi2 = __is_normal_iterator<_BI2>::__value;

     const bool __bi1 = _BI1::__value_1 < _BI1::__value_2;

     if (_BI1::__value_1 < _BI1::__value_2) {}

  // return std::__copy_backward_normal<__bi1, __bi2>::__copy_b_n(__first,__last,__result);
     return __result;
   }
