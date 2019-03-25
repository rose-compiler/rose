// This is similar to test2019_60.C
class A
   {
     public:
          static void func(int);
          template<void (*f)(int) > class X;
          X<func>* func2();
   };

A::X<A::func>* A::func2() {}
