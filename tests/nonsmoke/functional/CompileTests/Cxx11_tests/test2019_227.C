// This is similar to test2019_60.C
namespace A
   {
     class B;
     template<typename T> class int_temp;
   };

A::int_temp<A::B>* foobar();

namespace A
   {
     int_temp<B>* ::foobar();
   };


