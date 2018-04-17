
template < typename T>
void foo(T x)
   {
   }

void foobar()
   {
     int x;
  // foo(x);
     foo<int>(7);
  // foo(42);
   }
