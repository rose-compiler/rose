class S {
  int x = 0;
  void f() {
    int i = 0;
//  auto l1 = [i, x]{ use(i, x); };    // error: x is not a variable
    auto l2 = [i, x=x]{ use(i, x); };  // OK, copy capture
    i = 1; x = 1; l2(); // calls use(0,0)
    auto l3 = [i, &x=x]{ use(i, x); }; // OK, reference capture
    i = 2; x = 2; l3(); // calls use(1,2)
  }
};

