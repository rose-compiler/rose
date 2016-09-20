
namespace std {
  template<typename _Alloc>
    class allocator;
  template<class _CharT>
    struct char_traits;
  template<typename _CharT, typename _Traits = char_traits<_CharT>,
           typename _Alloc = allocator<_CharT> >
    class basic_string;

  typedef basic_string<char> string;
}

namespace std {
  template<typename _CharT, typename _Traits, typename _Alloc>
    class basic_string
    {
    public:
   // DQ (12/21/2014): convert this to two different constructors.
      basic_string(const _CharT* __s);

     ~basic_string() { }
  };

}

