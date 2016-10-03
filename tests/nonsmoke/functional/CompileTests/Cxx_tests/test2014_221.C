template < class T, class U > 
struct X 
   {
     X() {}           
   };

template < class T > struct X < T, T * > 
   {
     X() {}           
   };

void foobar()
   {
     X<int,int*> x;
   }
