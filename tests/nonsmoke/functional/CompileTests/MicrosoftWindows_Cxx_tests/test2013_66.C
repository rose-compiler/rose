class Y
   {
     friend class X;
     friend void foo();
     int x;
   };

#if 1
class X
   {
     void foobar() { Y y; y.x = 4; }
   };
#endif


