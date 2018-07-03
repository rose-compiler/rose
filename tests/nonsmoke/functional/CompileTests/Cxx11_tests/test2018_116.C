struct Struct1{ int x; };

template<typename T>
void foo(T&&);

void foobar()
   {
     foo (Struct1{ 42 });
   }
