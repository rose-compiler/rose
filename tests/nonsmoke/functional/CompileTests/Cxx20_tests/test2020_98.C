template< class T > void f(T);    // #1: overload for all types
template<>          void f(int*); // #2: specialization of #1 for pointers to int
template< class T > void f(T*);   // #3: overload for all pointer types
 
// DQ (7/21/2020): Moved function calls into a function.
void foobar()
   {
     f(new int(1)); // calls #3, even though specialization of #1 would be a perfect match
   }
