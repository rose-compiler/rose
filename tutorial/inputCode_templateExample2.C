// template function 
template <typename T>
void foo( T t )
   {
   }

// Specialization from user
template<> void foo<int>(int x) {}

int main()
   {
     foo(1);
   }

