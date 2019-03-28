// This is similar to test2019_60.C
namespace A
   {
     class B;
     template<typename T> class int_temp;
     typedef int_temp<B>* type1;
   };

A::int_temp<A::B>* var;

