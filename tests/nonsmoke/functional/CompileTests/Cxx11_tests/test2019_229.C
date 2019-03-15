// This is similar to test2019_60.C
namespace A
   {
     class B;
     template<typename T> class int_temp;

     namespace X
        {
          class B;
        }

   }

A::int_temp<A::X::B>* foobar();

namespace A
   {
     int_temp<X::B>* ::foobar();
   };


