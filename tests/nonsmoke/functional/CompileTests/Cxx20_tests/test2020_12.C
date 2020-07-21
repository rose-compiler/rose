
#if 0
void foo1()
   {
     for (auto& x : foo().items()) { /* .. */ } // undefined behavior if foo() returns by value
   }
#endif

// template <typename T> T foo();
int foo();

template <typename T>
void foo2()
   {
     for (T thing = foo(); auto& x : thing.items()) { /* ... */ } // OK
   }
