// Test of new gnu 3.x header files with ROSE

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
      friend class istreambuf_iterator<char_type, _Traits>;
    };

  template<typename _CharT, typename _Traits>
  class basic_filebuf : public basic_streambuf<_CharT, _Traits>
        {
          public:
               int foo();
        };

// The declaration contained in this forward declaration of 
// the specialization is shared with the "foo()" in the class.
   template<> int basic_filebuf<char,char>::foo();
}



