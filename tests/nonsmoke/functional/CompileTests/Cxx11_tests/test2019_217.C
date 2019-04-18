// This is similar to test2019_60.C
class A
   {
     public:
          static void func(int);
          template<void (*f)(int) > class X;
   };

using Y = A::X<A::func>;
