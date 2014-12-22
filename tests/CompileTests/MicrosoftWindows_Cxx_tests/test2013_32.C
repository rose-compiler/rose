namespace std 
   {
     template<typename _Alloc> class allocator;

     template<class _CharT> struct char_traits;

  // template<typename _CharT, typename _Traits = char_traits<_CharT>,typename _Alloc = allocator<_CharT> > class basic_string;
     template<typename _CharT> class basic_string;

     template<> struct char_traits<char>;

     typedef basic_string<char> string;
   }

#if 1

// namespace std __attribute__ ((__visibility__ ("default"))) {
namespace std {
// # 110 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/../../../../include/c++/4.2.4/bits/basic_string.h" 3
// template<typename _CharT, typename _Traits, typename _Alloc>
  template<typename _CharT>
    class basic_string
    {
//    typedef typename _Alloc::template rebind<_CharT>::other _CharT_alloc_type;

#if 0
 // MSVC does not allow this "name followed by "::" must be a class or namespace name"
    public:
      typedef _Traits traits_type;
      typedef typename _Traits::char_type value_type;
      typedef _Alloc allocator_type;
      typedef typename _CharT_alloc_type::size_type size_type;
      typedef typename _CharT_alloc_type::difference_type difference_type;
      typedef typename _CharT_alloc_type::reference reference;
      typedef typename _CharT_alloc_type::const_reference const_reference;
      typedef typename _CharT_alloc_type::pointer pointer;
      typedef typename _CharT_alloc_type::const_pointer const_pointer;
#endif
    };
}

#endif


namespace std 
   {
     string s;

     string foo();

     class X
        {
          string s;
          class Y
             {
               string s;
             };
        };
   }


   
