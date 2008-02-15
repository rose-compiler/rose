// Test for calls to sizeof()

// test template
template <typename T>
class X
   {
     public:
          T t;
   };

int main()
   {
     X<int> objectInt;

  // Demonstrates use of "sizeof()" which is substituted with the size at compile-time.
  // thus ROSE can't output the "sizeof()" function as the original code did.
     int x = sizeof(int);

  // DQ (1/17/2007): This is now fiex as a compiler specific code generation detail in the unparser!
  // #if ( (__GNUC__ > 3) || ( (__GNUC__ == 3) && (__GNUC_MINOR__ > 3) ) )
  // This fails for gnu version 3.3.x since the "X" will be unparse (correctly, I think) as "::X"
     int y = sizeof(X<int>);
  // #endif

     int z = sizeof(int*);

     return 0;
   }


