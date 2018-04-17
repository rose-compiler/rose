template < class T > 
struct X 
   {
//   X() { int abc; }           
   };

template < class T > struct X < T* > 
   {
     X() { int def; }
   };

void foobar()
   {
//   X<int> x_not_using_pointer;
     X<int*> x_using_pointer;
   }
