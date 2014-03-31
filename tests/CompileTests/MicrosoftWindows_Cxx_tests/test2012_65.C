template <typename T>
class X
   {
     public:
       // void foobar();
          void foobar();
   };

// DQ (2/20/2010): Newer versions of GNU g++ require "template<>"
// This is a template specialization (of the function for a specialized class of template X)
template<> void X<int>::foobar()
// void X<int>::foobar()
   {
   }


