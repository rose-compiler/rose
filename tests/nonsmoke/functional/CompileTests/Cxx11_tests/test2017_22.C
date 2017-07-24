template <class T> void foo(T t) {}

class className 
   {
     public:
          void bar(int value) 
             {
               foo<int>(value);
             }
   };
