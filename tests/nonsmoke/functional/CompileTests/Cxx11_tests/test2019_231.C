// This is similar to test2019_60.C
namespace A
   {
     namespace B
        {
          static void func(int);
          template<void (*f)(int) > class int_temp{};
        }
     B::int_temp<&B::func>* func2();
   }

A::B::int_temp<&A::B::func>* A::func2() 
   {
   }
