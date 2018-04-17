
class A
   {
     public:
          int findme;
          template < class T > void foo(T i) {}
   };

A a;

#if 1
template <> inline void A::foo(int i) {}
#endif

int main()
   {
     int i = 42;

  // This forces A::foo(T) to be instantiated
     a.foo(i);
   }

