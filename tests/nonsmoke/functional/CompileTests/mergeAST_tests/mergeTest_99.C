// This demonstrates a case of: SgTypedefDeclaration is not in parent's child list
// In this case the typedef is processed as a template argument and this causes
// the parent to be set incorrectly.  We fixed this to assign the parent to be the 
// template argument.

namespace __gnu_cxx
   {
     template<typename _Iterator, typename _Container> class __normal_iterator;
   }

namespace std
   {
     template<typename _Alloc> class allocator;
     template<class _CharT> struct char_traits;

  // Note that char_traits<_CharT> will be built as a non-defining template declaration
     template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT> > class basic_string;

//   }
//namespace std {

     template<typename _Tp> class allocator;

     template<> class allocator<void>
        {
          public:
            // typedef void* pointer;
               typedef const void* const_pointer;
        };

     template<typename _Tp> class allocator
        {
          public:
            // typedef _Tp* pointer;
               typedef const _Tp* const_pointer;
        };

// }
// namespace std {

     template<typename _CharT, typename _Traits, typename _Alloc> class basic_string
        {
          public:
            // typedef typename _Alloc::pointer pointer;
               typedef typename _Alloc::const_pointer const_pointer;
            // int x;
            // typedef struct ABC {} DEF;

            // typedef __gnu_cxx::__normal_iterator<pointer, basic_string> iterator;
               typedef __gnu_cxx::__normal_iterator<const_pointer, basic_string> const_iterator
        };
   }

namespace std
   {
     extern template class basic_string<char>;
   }

