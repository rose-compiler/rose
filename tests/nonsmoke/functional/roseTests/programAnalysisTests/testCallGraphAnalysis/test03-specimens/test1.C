// #include <iostream>
// #include <sstream>
// #include <stdio.h>
// #include <stdlib.h>

class A
   {
     public:
	       int f1() { return 1; };
          int f2() { pf = &A::f1; return (this->*pf)(); }
          int (A::*pf) ();
   };

