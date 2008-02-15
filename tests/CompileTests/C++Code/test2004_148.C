
// Test of Initialized names within variable declaarations where the type is a template argument

namespace boost
   {
     template <typename T>
     class A
        {
          T * ptr;
        };
   }

boost::A<int> a;
