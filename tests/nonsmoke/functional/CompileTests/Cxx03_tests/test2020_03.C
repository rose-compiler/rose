class hash_map
   {
  // EDG will accept this, but ROSE will not output the "[[no_unique_address]]" 
  // attribute yet, and so it will compile with g++ in C++03 mode. If we output
  // the "[[no_unique_address]]" attribute then g++ 6.1 in -std=c++03 mode will 
  // fail.  with g++ 6.1 and -std=c++ it will pass fine, because g++ defaults to 
  // C++11 or C++14.  Using -std=c++ causes no use of the -std option when calling
  // the backend compiler.

  // DQ (8/18/2020): I think this is C++11, or C++14 code that does not wotk with GNU 4.9 using C++11 support.
  // [[no_unique_address]] int x;
     int x;
   };

