class X
   {
     public:
          bool operator>(const X& x);
          bool operator<(const X& x);
      
   };

class Y
   {
     public:
          bool operator>(const Y& x);
          bool operator<(const Y& x);
      
   };


template <class T, class U>
struct less_than_comparable2
   {
     friend bool operator<=(const T& x, const U& y) { return !static_cast<bool>(x > y); }
     friend bool operator>=(const T& x, const U& y) { return !static_cast<bool>(x < y); }
     friend bool operator>(const U& x, const T& y)  { return y < x; }
     friend bool operator<(const U& x, const T& y)  { return y > x; }
//   friend bool operator<=(const U& x, const T& y) { return !static_cast<bool>(y < x); }
//   friend bool operator>=(const U& x, const T& y) { return !static_cast<bool>(y > x); }
   };

void foo()
   {
     less_than_comparable2<X,X> x;
     less_than_comparable2<Y,Y> y;

     less_than_comparable2<X,Y> a1;
     less_than_comparable2<Y,X> a2;
   }

