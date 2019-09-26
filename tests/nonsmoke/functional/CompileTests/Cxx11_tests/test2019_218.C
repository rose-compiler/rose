// This is similar to test2019_60.C
namespace A
   {
     static void func(int);
     template<void (*f)(int) > class X;

     extern X<func>* variable;
   };

extern A::X<A::func>* A::variable;
