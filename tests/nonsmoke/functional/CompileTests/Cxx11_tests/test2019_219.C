// This is similar to test2019_60.C
namespace A
   {
     static void func(int);
     template<void (*f)(int) > class X;
     X<func>* variable;
   };

A::X<A::func>* variable_2;
