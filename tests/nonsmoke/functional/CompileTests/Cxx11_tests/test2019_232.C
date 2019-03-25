// This is similar to test2019_60.C
namespace C
   {
     class D;
   }

namespace A
   {
     class B;

     template<typename T, typename S> class int_temp;
     int_temp<B,C::D>* foobar();
   }

A::int_temp<A::B,C::D>* A::foobar();

