struct SomeType { int member; };

void foo()
   {
     sizeof(SomeType::member); // Does not work with C++03. Okay with C++11
   }
