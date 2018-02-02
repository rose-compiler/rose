
struct X
   {
  // This should be possible since a struct and a class are the same in C++.
  // However, niether GNU nor ROSE permit's this construction.
     virtual int foobar() = 0;
   };

