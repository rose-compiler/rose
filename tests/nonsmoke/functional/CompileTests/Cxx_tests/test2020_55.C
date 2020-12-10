// This test code demonstrates two ways of handling operator==().
// It is trying to replicate a bug demonstrated in test2007_141.C,
// but within a simplere code.

class X
   {
     public:
          friend bool operator==(const X & x1, const X & x2);
   };

void foo()
   {
     X x;

     operator==(x,x);
   }
