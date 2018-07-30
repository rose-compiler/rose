template<typename T>
void foo(T&&);

struct Struct1{};

void foobar()
   {
     foo (Struct1{});
   }
