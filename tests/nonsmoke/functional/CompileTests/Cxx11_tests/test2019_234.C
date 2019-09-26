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

namespace C
   {
  // A::int_temp<A::B,D>* foobar();
     A::int_temp<A::B,D>* foobar();
   }

namespace E
   {
  // A::int_temp<A::B,D>* foobar();
     A::int_temp<A::B,C::D>* foobar();
   }
