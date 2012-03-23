
template <typename T>
struct X
   {
     struct Y
        {
          void foo();
        };
   };


X<int> x;
X<int>::Y y;

void foobar()
   {
  // This should unparse to be: "y.foo();"
     y.foo();
   }



