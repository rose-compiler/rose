// This is similar to test2019_60.C
namespace A
   {
     class B;
     template<typename T> class int_temp;
   };

A::int_temp<A::B>* var;

namespace B
   {
  // int_temp<A::B>* var;

     namespace A
        {
          ::A::int_temp<::A::B>* var;
        }
   }

