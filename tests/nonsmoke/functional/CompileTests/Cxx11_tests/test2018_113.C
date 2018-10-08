
struct Struct1{};

// template<typename T>
void foo(Struct1&&);

void foobar()
   {
     foo (Struct1{});
   }
