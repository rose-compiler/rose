namespace std 
   {
     template<typename> class allocator;

     template<class _CharT> struct char_traits;

     template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT> > class basic_string;

     template<> struct char_traits<char>;

     typedef long int	ptrdiff_t;
     typedef ptrdiff_t	streamsize; 

     template<typename _CharT, typename _Traits = char_traits<_CharT> > class basic_streambuf;

     typedef basic_streambuf<char> streambuf;

     template<typename _CharT, typename _Traits>
     streamsize __copy_streambufs_eof(basic_streambuf<_CharT, _Traits>*, basic_streambuf<_CharT, _Traits>*, bool&);  
 
     template<typename _CharT, typename _Traits>
     class basic_streambuf 
        {
          public:
               typedef _CharT 					char_type;
               typedef _Traits 					traits_type;
 #if 1
               typedef typename traits_type::int_type 		int_type;
#endif

               friend streamsize __copy_streambufs_eof<>(basic_streambuf*, basic_streambuf*, bool&);

          public:
               virtual ~basic_streambuf() { }

          protected: 
               basic_streambuf() { }    
        };

     template<>
     streamsize __copy_streambufs_eof(basic_streambuf<char>* __sbin, basic_streambuf<char>* __sbout, bool& __ineof);

     template<>
     streamsize __copy_streambufs_eof(basic_streambuf<wchar_t>* __sbin, basic_streambuf<wchar_t>* __sbout, bool& __ineof);
   }

namespace std 
   {
     template<typename _CharT, typename _Traits>
     streamsize __copy_streambufs_eof(basic_streambuf<_CharT, _Traits>* __sbin, basic_streambuf<_CharT, _Traits>* __sbout, bool& __ineof)
        {
          streamsize __ret = 0;
          typename _Traits::int_type __c;
          return __ret;
        }

     extern template
     streamsize __copy_streambufs_eof(basic_streambuf<char>*, basic_streambuf<char>*, bool&);

     extern template
     streamsize __copy_streambufs_eof(basic_streambuf<wchar_t>*, basic_streambuf<wchar_t>*, bool&);
   } 


