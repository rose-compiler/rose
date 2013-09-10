// This test code demonstrates two ways of handling operator==().
// It is trying to replicate a bug demonstrated in test2007_141.C,
// but within a simplere code.

class X
   {
     public:
          friend bool operator==(const X & x1, const X & x2);
   };


class Y
   {
     public:
          bool operator==(const Y & y);
   };

void foo()
   {
     X x;
     Y y;

     operator==(x,x);
     (x == x);
     
     y.operator==(y);
     (y == y);

     int a;
     a = (x == x);

  // This case reproduces the bug demonstrated by test2007_141.C (on 4/20/2013).
     a = (y == y);
   }
