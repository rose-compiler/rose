namespace std {

#if 0
// Forward declaration of templated class
   template< typename _CharT, typename _Traits > class istreambuf_iterator;

// The templated class
   template< typename _CharT, typename _Traits > class basic_streambuf
    {
      int xyz1;
      public:
      typedef _CharT 					char_type;
      int xyz;

   // A forward declaration of a specialization of a class
      friend class istreambuf_iterator<char_type, _Traits>;
    };
#endif

  template<typename _CharT, typename _Traits>
  class basic_filebuf // : public basic_streambuf<_CharT, _Traits>
        {
          public:
               int foo();
        };

#if 0
// The declaration contained in this forward declaration of 
// the specialization is shared with the "foo()" in the class.
   template<> int basic_filebuf<char,char>::foo();
#endif

#if 1
   template<> int basic_filebuf<char,char>::foo() 
      {
        return 0;
      }
#endif
}


