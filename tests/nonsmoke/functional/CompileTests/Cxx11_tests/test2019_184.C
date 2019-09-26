template <class T> struct A_ 
   {
     struct B_ { typedef T bt; };
     struct C_ : B_ { typedef T ct; };
   };

void foobar()
   {
     int a = sizeof(A_<char>::B_::bt);
     int b = sizeof(A_<char>::C_::ct);
   }


