
// This is a shorted test of the same bug as represented in test2009_35.C (by Liao)

namespace std
   {
     template<typename _CharT>
     class basic_ostream
        {
          public:
               basic_ostream<_CharT> & operator<<(basic_ostream<_CharT> & (*__pf)( basic_ostream<_CharT> &));
        };

     typedef basic_ostream<char> ostream;

     extern ostream cout;

     template <typename _CharT> basic_ostream<_CharT> & endl ( basic_ostream<_CharT> & __os)
        {
          return __os;
       // return endl(__os);
        }
#if 0
  // If this is uncommented then this test code will compile.
     extern template ostream & endl (ostream&);
#endif
   }

class TestClass
   {
     public:
          void foo()
             {
               std::cout << std::endl;
             }
   };
