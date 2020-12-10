// This is similar to test2019_60.C
namespace A
   {
     class B;
     template<typename T> class int_temp;
   };

#if 0
namespace C
   {
     class B;
   };
#endif

A::int_temp<A::B>* var1;

#if 1
namespace A
   {
     int_temp<B>* var2;
   };
#endif

