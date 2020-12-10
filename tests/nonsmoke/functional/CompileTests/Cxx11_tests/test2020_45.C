struct A;
template<class T> int foo(T t, A a);

struct A 
   {
     friend int foo<>(int, A);
   };
