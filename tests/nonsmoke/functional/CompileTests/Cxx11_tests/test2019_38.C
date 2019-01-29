template <class T> int c(T const& t);

// This line will not compile with GNU version 5.1 (at least).
template <class T> int e(int xxxxx = c( []{} )) { }

void foo() 
   {
     e<long>();
   }

