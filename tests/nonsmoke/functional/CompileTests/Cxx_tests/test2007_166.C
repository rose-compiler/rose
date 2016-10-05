// This is a match for copytest2007_67.C

template <typename T>
class X
   {
     public:
          void foobar(T t);
   };

void foo()
   {
     X<int> a;

     a.foobar(1);
   }

