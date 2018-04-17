// Liao, 12/7/2016

// extracted from /usr/include/c++/4.4.7/bits/stl_algobase.h(701,7)
template<typename _ForwardIterator, typename _Tp>
//    inline typename
//    __gnu_cxx::__enable_if<__is_scalar<_Tp>::__value, void>::__type
void 
fill(_ForwardIterator __first, _ForwardIterator __last,
    const _Tp& __value)
{
  const _Tp __tmp = __value;
  for (; __first != __last; ++__first)
    *__first = __tmp;
}

//extracted from loopsuite/DelVolBaseLoopAlgorithm.cxx
void DelVolBaseLoopAlgorithm_setup (int nnalls)
{
 double * m_dvol = new double [nnalls] ; 
 fill (m_dvol, m_dvol+nnalls, 0.0);
  
}
