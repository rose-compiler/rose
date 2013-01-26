
  // template<typename _Alloc> class allocator;

     template<class _CharT> struct char_traits;

//   template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT> > class basic_string;
//   template<typename _CharT> class basic_string;
     template<typename _CharT, typename _Traits = char_traits<_CharT> > class basic_string;

  // template<> struct char_traits<char>;

  // typedef basic_string<char> string;

  // template<typename _CharT, typename _Traits, typename _Alloc>
  // template<typename _CharT>
     template<typename _CharT, typename _Traits>
     class basic_string
        {
          public:
            // inline basic_string();
               basic_string();
        };

  // This line causes a problem for code that ROSE generates.
  // ROSE unparses: 
  //    template<> class basic_string < char , char_traits< char > , allocator< char > >  ;
  //    template<> class basic_string < char , char_traits< char > >  ;
  // With string typedef commented out we generate (working):
  //    template class basic_string < char , char_traits< char > >  ;
  // instead of:
  //    extern template class basic_string<char>;

     extern template class basic_string<char>;

  // This is also an error for ROSE.
  // template<> class basic_string < char , char_traits< char > , allocator< char > >  ;


// string _M_msg;
basic_string<char> _M_msg;

