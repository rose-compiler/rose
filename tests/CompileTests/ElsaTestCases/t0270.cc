// t0270.cc
// minimized from ostream, basic_string<char> ctor

template < typename _Tp > 
class allocator
{
  typedef int _Alloc;
};

template < typename _CharT, 
           typename _Alloc >
class basic_string {
public:
  struct _Alloc_hider : _Alloc {
    _Alloc_hider (const _Alloc & __a)
      : _Alloc (__a)
    { }
    _CharT *_M_p;
  };

  _Alloc_hider _M_dataplus;

  basic_string ();
};

template < typename _CharT,
           typename _Alloc >
basic_string < _CharT, _Alloc >::basic_string ()
  : _M_dataplus (_Alloc())
{ }

template class basic_string < char, allocator<char> >;
