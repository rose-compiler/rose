// This is similar to test2019_60.C
namespace A
   {
     static void func(int);
     template<void (*f)(int) > class int_temp{};
     int_temp<func>* func2();
   };

A::int_temp<&A::func>* A::func2() 
   {
   }
