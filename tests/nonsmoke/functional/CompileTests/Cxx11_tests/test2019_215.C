// This is similar to test2019_60.C
class A
   {
     public:
          typedef int var;
          template<typename T> class int_temp;
          int_temp<var>* func2();
   };

// A::int_temp<A::func>* A::func2() 
A::int_temp<A::var>* A::func2() 
   {
   }
