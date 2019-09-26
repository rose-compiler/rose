template <class T> struct A_ 
   {
  // struct B_ { typedef T bt; };
     struct B_ { int bt; };
   };

void foobar()
   {
  // unparsed as:
  // int a = (sizeof(A_< char > ::B_< > ::bt ));
     int a = sizeof(A_<char>::B_::bt);
   }


