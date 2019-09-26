
#if 1
template <class... Args>
int f (const int x, const int y, Args&&... args) {
  // Do something
}
#endif

template <typename T> void foo(T&& ) { }

void foobar()
   {
     foo(1); // T is int
     int x;
     foo(x); // T is int&

     f(1,2,3,4);

     foo(f(1,2,3,4));
   }

