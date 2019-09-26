class A 
   {
     class A1{};
     static void func(A1, int);
     static void func(float, int) {}
     static const int garbconst = 3;
     public:
          template<class T, int i, void (*f)(T, int) > class int_temp{};
          int_temp<A1, 5, func>* func2();
   };

// BUG: template argument "&A::func" is unparsed as "func"
// and thus unparsed as: 
// A::int_temp< class A::A1  , 5 , func > *A::func2()
   A::int_temp<A::A1, A::garbconst + 2, &A::func>* A::func2() 
   {
     static A::int_temp<A::A1, A::garbconst + 2, &A::func>* p = 0;
     return p;
   }

void foobar()
   {
  // chk(1);	
   }
