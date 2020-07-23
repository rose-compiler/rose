
template<class X> void f(X a);
template<class X> void f(X* a);
int* p;

// DQ (7/22/2020): Needed to add function to wrap the function call.
void foobar()
   {
     f(p);
   }

