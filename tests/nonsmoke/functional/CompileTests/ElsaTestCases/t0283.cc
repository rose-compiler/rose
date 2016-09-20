// t0283.cc
// delta-minimized from ostream, not yet hand-minimized

namespace std
{
  template < typename _Alloc > 
  class allocator;

  template < class _CharT >
  struct char_traits;

  template < typename _CharT,
             typename _Traits = char_traits < _CharT >,
             typename _Alloc = allocator < _CharT > >
  class basic_string;
}

typedef int ptrdiff_t;
typedef unsigned size_t;

struct __false_type {} ;

template < class _Tp > 
struct _Is_integer {
  typedef __false_type _Integral;
};

namespace std
{
  struct input_iterator_tag {};
  struct forward_iterator_tag : input_iterator_tag {};
  struct bidirectional_iterator_tag : forward_iterator_tag {};
  struct random_access_iterator_tag : bidirectional_iterator_tag {};

  template < typename _Category,
             typename _Tp,
             typename _Distance,
             typename _Pointer = _Tp,
             typename _Reference = _Tp >
  struct iterator {
    typedef _Category iterator_category;
  };

  template < typename _Iterator > 
  struct iterator_traits {
    typedef typename _Iterator::iterator_category iterator_category;
  };

  template < typename _Tp > 
  struct iterator_traits <_Tp * > {
    typedef random_access_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef ptrdiff_t difference_type;
    typedef _Tp * pointer;
    typedef _Tp reference;
  };
}

namespace __gnu_cxx
{
  using std::iterator_traits;
  using std::iterator;

  template < typename _Iterator, 
             typename _Container > 
  class __normal_iterator 
    : public iterator < typename iterator_traits < _Iterator >:: iterator_category,
                        typename iterator_traits < _Iterator >:: value_type,
                        typename iterator_traits < _Iterator >:: difference_type,
                        typename iterator_traits < _Iterator >::pointer, 
                        typename iterator_traits < _Iterator >::reference >
  { };
}

namespace std
{
  template < int __inst > 
  class __default_alloc_template {
  };

  template < typename _Tp > 
  class allocator {
  public:
    typedef size_t size_type;
    typedef _Tp * pointer;
  };

  template < typename _T1, 
             typename _T2 > 
  bool operator== (allocator < _T1 >, allocator < _T2 >)
  { }
}

namespace std
{
  template < class _Arg2, 
             class _Result > 
  struct binary_function {
  };

  template < typename _CharT, 
             typename _Traits, 
             typename _Alloc > 
  class basic_string {
    typedef typename _Alloc::size_type size_type;
    typedef typename _Alloc::pointer pointer;
    typedef __gnu_cxx::__normal_iterator < pointer, basic_string > iterator;

    struct _Alloc_hider : _Alloc {
      _Alloc_hider (_CharT *, _Alloc)
      { }
    };

  public:
    static const size_type npos = static_cast < size_type > (-1);

  private:
    _Alloc_hider _M_dataplus;

    iterator _M_check (size_type) { }
    iterator _M_fold (size_type, size_type) const { }
    basic_string (const basic_string &, size_type, size_type);
    size_type size () { }

    template < class _InIter >
    static _CharT * _S_construct_aux
      (_InIter __beg, _InIter __end, _Alloc __a, __false_type)
    {
      typedef typename iterator_traits < _InIter >::iterator_category _Tag;
      _S_construct (__beg, __end, __a, _Tag ());
    }

    template < class _InIter >
    static _CharT * _S_construct (_InIter __beg, _InIter __end, _Alloc __a)
    {
      typedef typename _Is_integer < _InIter >::_Integral _Integral;
      _S_construct_aux (__beg, __end, __a, _Integral ());
    }

    template < class _InIter >
    static _CharT * _S_construct (_InIter, _InIter, _Alloc, input_iterator_tag);
  };

  template <typename _CharT,
            typename _Traits,
            typename _Alloc>
  basic_string<_CharT,_Traits,_Alloc>::basic_string
    (const basic_string &__str, size_type __pos, size_type __n)
    : _M_dataplus(_S_construct(_M_check (__pos),
                               __str._M_fold (__pos, __n),
                               _Alloc ()),
                  _Alloc ())
  { }

  template class basic_string < char >;
}
