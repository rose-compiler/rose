template < class T > void foo(T i) {}

int main()
   {
     int i = 42;

  // This forces foo(T) to be instantiated
     foo(i);
   }

