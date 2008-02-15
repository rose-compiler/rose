// Test of new gnu 3.x header files with ROSE

// this test code demonstrates an error:
// preprocessor: /home/dquinlan/ROSE/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:18121: SgFunctionDeclaration* sage_gen_routine_decl(int, a_boolean*): Assertion `curr_source_sequence_entry != __null' failed.


// namespace std {

  template<class _CharT> struct char_traits;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_filebuf;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class istreambuf_iterator;

  template<typename _CharT, typename _Traits> int __copy_streambufs();
  
  template<typename _CharT, typename _Traits>
    class basic_streambuf 
    {
    public:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;

      friend class istreambuf_iterator<char_type, traits_type>;
    };

     template<typename _CharT> class __basic_file;

     template<> class __basic_file<char>
        {
          public:
               __basic_file();
        };

     template<typename _CharT, typename _Traits>
     class basic_filebuf : public basic_streambuf<_CharT, _Traits>
        {
          public:
               int foo();
        };

     template<> int basic_filebuf<char>::foo();
//  }


