// This is similar to test2019_60.C
namespace A
   {
     class B;
     template<typename T> class int_temp;
  // typedef int_temp<B>* type1;
   };

typedef A::int_temp<A::B>* type1;

namespace A
   {
     typedef int_temp<B>* type1;
   };

