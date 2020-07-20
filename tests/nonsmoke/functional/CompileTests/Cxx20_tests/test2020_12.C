
foo1()
   {
     for (auto& x : foo().items()) { /* .. */ } // undefined behavior if foo() returns by value
   }

foo2()
   {
     for (T thing = foo(); auto& x : thing.items()) { /* ... */ } // OK
   }
