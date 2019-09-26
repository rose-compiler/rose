void example(int, int) {}

void foobar()
   {
  // type alias, identical to
  // typedef void (*func)(int, int);
     using func = void (*) (int, int);
  // the name 'func' now denotes a pointer to function:
  // void example(int, int) {}
     func f = example;
   }

