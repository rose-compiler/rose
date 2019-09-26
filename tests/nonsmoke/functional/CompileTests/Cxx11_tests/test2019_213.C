class A 
   {
          class A1{};
          static void func(A1, int);
          static void func(float, int); // {}
       // static const int garbconst = 3;

     public:
          template<class T, void (*f)(T, int) > class int_temp{};
          int_temp<A1, func>* func2();
   };

A::int_temp<A::A1, &A::func>* A::func2() 
   {
   }
