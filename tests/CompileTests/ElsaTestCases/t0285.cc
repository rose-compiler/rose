// t0285.cc
// assertion failure: canAcceptNames

namespace std
{
  template < class _CharT > struct char_traits;
}
typedef int ptrdiff_t;
extern "C"
{
  typedef ptrdiff_t streamsize;
}
namespace std
{
  template < typename _CharT, typename _Traits =
    char_traits < _CharT > >class basic_ios;
  template < typename _CharT, typename _Traits =
    char_traits < _CharT > >class basic_streambuf;
}
extern "C++"
{
}
typedef int _Atomic_word;
static inline _Atomic_word
__exchange_and_add (volatile _Atomic_word * __mem, int __val)
{
}

namespace std
{
  enum _Ios_Fmtflags
  {
    _M_ios_fmtflags_end = 1L << 16
  };
    template < typename _CharT,
    typename _Traits > streamsize __copy_streambufs (basic_ios < _CharT,
						     _Traits > &_ios,
						     basic_streambuf < _CharT,
						     _Traits > *__sbin,
						     basic_streambuf < _CharT,
						     _Traits > *__sbout);
    template < typename _CharT, typename _Traits > class basic_streambuf
  {
  public:typedef _CharT char_type;
    typedef _Traits traits_type;
    typedef basic_streambuf < char_type, traits_type > __streambuf_type;
    friend streamsize __copy_streambufs <> (basic_ios < char_type,
					    traits_type > &__ios,
					    __streambuf_type * __sbin,
					    __streambuf_type * __sbout);
  };
  //extern    // let it go through in non-gnu mode
  template class basic_streambuf < char >;
}
