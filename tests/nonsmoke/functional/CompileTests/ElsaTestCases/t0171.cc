// t0171.cc
// test from Rob van Behren: Expression nonterm is ambiguous
             

template <class _T1, class _T2>
struct pair {
  pair(_T1, _T2);
};

template <class _T1, class _T2>
inline pair<_T1, _T2> make_pair(const _T1& __x, const _T2& __y)
{
  return pair<_T1, _T2>(__x, __y);
}
