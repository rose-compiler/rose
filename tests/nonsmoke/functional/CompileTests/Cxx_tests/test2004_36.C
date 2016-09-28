
class A_4_36
   {
     public:
          int findme;
          template < class T > void foo(T i) {}
   };

A_4_36 a;

#if 1
template <> inline void A_4_36::foo(int i) {}
#endif

int main()
   {
     int i = 42;

  // This forces A::foo(T) to be instantiated
     a.foo(i);
   }

