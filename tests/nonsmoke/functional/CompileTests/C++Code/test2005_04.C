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
     int y = sizeof(X<int>);
     int z = sizeof(int*);

     return 0;
   }
