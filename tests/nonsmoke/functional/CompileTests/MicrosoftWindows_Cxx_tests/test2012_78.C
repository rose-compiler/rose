class A
   {
     public:
  // int x;

     template<class T> void g(int y, T u);
   };

#if 1
template<class T>
void A::g(int y, T u)
   {
//   x = x + y + u;
   }
#endif

int main()
   {
     int t=0, r=3;
     A a;
       a.g<int>(t,r);
   }
