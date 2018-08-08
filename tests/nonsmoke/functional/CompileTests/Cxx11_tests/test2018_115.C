struct Struct1{ int x; };

// template<typename T>
void foo(Struct1&&);

void foobar()
   {
     foo (Struct1{42});

  // Also allowed withouth class specifier (since it is not a template function)
  // foo ({42});
   }
