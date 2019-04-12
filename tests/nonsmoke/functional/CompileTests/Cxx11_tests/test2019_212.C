class A 
   {
          class A1{};
          static void func(A1, int);
          static void func(float, int) {}
          static const int garbconst = 3;

     public:
          template<class T, int i, void (*f)(T, int) > class int_temp{};
       // template<> class int_temp<A1, 5, func> { void func1(); };
       // friend int_temp<A1, 5, func>::func1();
          int_temp<A1, 5, func>* func2();
   };

A::int_temp<A::A1, A::garbconst + 2, &A::func>* A::func2() 
   {
     static A::int_temp<A::A1, A::garbconst + 2, &A::func>* p = 0;
     return p;
   }
