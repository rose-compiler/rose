
// Test use of >> syntax in C++11 support.

template < typename T > 
struct X
   { 
     T a; 
   };

template < typename T > 
struct Y
   { 
     T a; 
   };

X<Y<int>> b;
