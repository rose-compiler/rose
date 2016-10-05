// This is a copy of test2004_67.C (with additions).

// These are GNU 3.3.x specific attributes.
// Force these to go away since we can't handle them yet (Need to turn on support in EDG for these)
// #define __attribute__(name)

// This the the header file which demonstrates a problem for ROSE presently!
// #include <fstream>

namespace std {

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
   // NOTE: when output within the specialization we need to output 
   // "friend class istreambuf_iterator< char , char  > ;" 
   // or we need to output the typedef!
      friend class istreambuf_iterator<char_type, _Traits>;
    };

  template<typename _CharT, typename _Traits>
  class basic_filebuf : public basic_streambuf<_CharT, _Traits>
        {
          public:
               int foo();
        };

#if 0
// This will compile for GNU g++ 4.4, however the ROSE generated code does not compile with 
// GNU g++ 4.4 (because the class template specialization is output, I think), might be 
// related to why Markus suggested it will fail for GNU g++ 4.8.

// The declaration contained in this forward declaration of 
// the specialization is shared with the "foo()" in the class.
   template<> int basic_filebuf<char,char>::foo();
#endif

#if 1
// This will compile with GNU g++ 4.4.
   template<> int basic_filebuf<char,char>::foo() 
      {
        return 0;
      }
#endif
}



