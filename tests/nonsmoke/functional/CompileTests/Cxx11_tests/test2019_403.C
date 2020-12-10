// This is similar to test2019_225.C
namespace A
   {
     class B;
     template<typename T> class int_temp {};
   };

A::int_temp<A::B> var;

namespace A
   {
  // This 2nd use of the type overwrites the first use of the type.
     int_temp<B> var;
   };

