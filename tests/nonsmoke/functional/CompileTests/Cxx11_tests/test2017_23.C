template <class T> void foo(T t) {}

void bar(int value) 
   {
     foo<int>(value);
   }
