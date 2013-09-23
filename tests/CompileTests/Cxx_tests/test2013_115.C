// This test code demonstrates two ways of handling operator==().
// It is trying to replicate a bug demonstrated in test2007_141.C,
// but within a simplere code.

class ABC {};

template <class T>
class X
   {
     public:
          friend bool operator==(const X<T> & x1, const X<T> & x2);
   };


template <class T>
class Y
   {
     public:
          bool operator==(const Y<T> & y);
   };

void foo()
   {
     X<ABC> x;
     Y<ABC> y;

     operator==(x,x);
     (x == x);
     
     y.operator==(y);
     (y == y);
   }
