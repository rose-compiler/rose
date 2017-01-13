template <typename T>
class X
   {
     public:
          void foobar(T t){};
   };

void foo()
   {
     X<int> a;

     a.foobar(1);
   }

