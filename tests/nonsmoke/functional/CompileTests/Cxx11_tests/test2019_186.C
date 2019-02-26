template <class T> struct A 
   {
     struct B { int bt; };
   };

void foobar()
   {
  // unparsed as:
  // int a = (sizeof(A< char > ::B< > ::bt ));
     int a = sizeof(A<char>::B::bt);
   }

