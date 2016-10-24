// This is a simplified version of test2004_67.C which was failing
// when the type of the SgTypeModifier::gnu_attribute_alignment was 
// a "short".  It might be that I didn't completely recompile ROSE
// after changing to thes from "unsigned long" or it MIGHT be that
// short is a problem for ROSETTA to handled.  I changed the type to
// "int" so that it could be a signed type and was more careful to
// recompile everything and now it all appears to work (no more 
// strange errors).


// namespace std {

// Forward declaration of templated class
// template< typename _CharT, typename _Traits > class istreambuf_iterator;

// The templated class
   template< typename _CharT, typename _Traits > class basic_streambuf
    {
//    int xyz1;
      public:
//    typedef _CharT 					char_type;
//    int xyz;

   // A forward declaration of a specialization of a class
//    friend class istreambuf_iterator<char_type, _Traits>;
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
// }



