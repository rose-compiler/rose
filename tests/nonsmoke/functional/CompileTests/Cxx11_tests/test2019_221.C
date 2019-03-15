// This is similar to test2019_60.C
namespace A
   {
     class B;
     template<typename T> class int_temp;
     int_temp<B>* func2();
   };

// A::int_temp<A::func>* A::func2() 
A::int_temp<A::B>* A::func2()
   {
   }
