class X
   {
     public:
          static int f() { return 3; }
   };

template<class T>
class Y 
   {
     friend X;
     friend class Z;
     friend T;
     public:
          int a() { return X::f(); }
          int b();
          int c() { return T::h(); }
   };

class Z
   {
     public:
          static int g() { return 4; }
   };

class W
   {
     public:
          static int h() { return 5; }
   };

template<> int Y<int>::b() 
   {
     return Z::g();
   }

void foobar()
   {
     Y<W> a;
     int b = a.c();
     Y<int> c;
     int d = c.a();
     int e = c.b();
   }
