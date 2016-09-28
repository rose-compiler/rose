template <typename T>
class X
   {
     public:
#if 1
          template <typename S> void foobar(S t){};
#endif
   };

void foo()
   {
     X<int> a;
   }


