// t0288.cc
// "ambiguous function template instantiation"

// 2005-08-03: This appears to be fixed by the switch to
// the new mtype module.

namespace std
{
  template < class _CharT > struct char_traits;
}
typedef int ptrdiff_t;
extern "C"
{
  typedef struct __locale_struct
  {
  }
   *__locale_t;
};
typedef struct __pthread_attr_s
{
}
pthread_barrierattr_t;
namespace std
{
  typedef ptrdiff_t streamsize;
    template < typename _CharT, typename _Traits =
    char_traits < _CharT > >class basic_ios;
    template < typename _CharT, typename _Traits =
    char_traits < _CharT > >class basic_streambuf;
}
extern "C++"
{
  namespace std
  {
    class exception
    {
    };
  }
}
namespace std
{
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
  template < typename _CharT,
    typename _Traits > streamsize __copy_streambufs (basic_ios < _CharT,
						     _Traits > &__ios,
						     basic_streambuf < _CharT,
						     _Traits > *__sbin,
						     basic_streambuf < _CharT,
						     _Traits > *__sbout)
  {
    try
    {
    }
    catch (exception & __fail)
    {
    }
  }
  extern template streamsize __copy_streambufs (basic_ios < wchar_t > &,
						basic_streambuf < wchar_t > *,
						basic_streambuf < wchar_t >
						*);
}
