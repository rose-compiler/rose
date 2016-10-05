

template < typename T >
struct X
   {
     typedef T T1;
     typedef T1 T2;
     typedef T2 T3;
   };

struct Y
   {
     X<X<int>::T3> x;
   };

