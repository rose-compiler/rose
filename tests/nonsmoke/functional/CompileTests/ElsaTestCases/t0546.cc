// t0546.cc
// error during disambiguation triggers assertion failure

namespace std
{
  template < class _CharT > struct char_traits;
}
typedef int ptrdiff_t;
namespace std
{
  typedef ptrdiff_t streamsize;
  class locale
  {
    class facet;
  };
  class locale::facet
  {
  };
  class ios_base
  {
  };
}
namespace std
{
  template < typename _CharT, typename _Traits > struct __pad
  {
  };
    template < typename _CharT,
    typename _OutIter > class num_put:public locale::facet
  {
  public:typedef _CharT char_type;
    void _M_pad (char_type __fill, streamsize __w, ios_base & __io,
		 char_type * __new, const char_type * __cs, int &__len) const;
  };
  template < typename _CharT, typename _OutIter > void num_put < _CharT,
    _OutIter >::_M_pad (_CharT __fill, streamsize __w, ios_base & __io,
			_CharT * __new, const _CharT * __cs, int &__len) const
  {
    __pad < _CharT, char_traits < _CharT > >::_S_pad (__io, __fill, __new,
						      __cs, __w, __len, true);
  }
}
