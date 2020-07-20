class S {
  int x = 0;
  void f() {
    int i = 0;
    auto l1 = [=]{ use(i, x); }; // captures a copy of i and a copy of the this pointer
    i = 1; x = 1; l1(); // calls use(0,1), as if i by copy and x by reference
    auto l2 = [i, this]{ use(i, x); }; // same as above, made explicit
    i = 2; x = 2; l2(); // calls use(1,2), as if i by copy and x by reference
    auto l3 = [&]{ use(i, x); }; // captures i by reference and a copy of the this pointer
    i = 3; x = 2; l3(); // calls use(3,2), as if i and x are both by reference
    auto l4 = [i, *this]{ use(i, x); }; // makes a copy of *this, including a copy of x
    i = 4; x = 4; l4(); // calls use(3,2), as if i and x are both by copy
  }
};


