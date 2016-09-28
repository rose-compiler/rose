// t0122.cc
// member/non-member tests

int f();

//ERROR(1): int g() const;

class X {
  int h();
  int h2() const;
  int h3() volatile;

  int (*i)();

  int j(int k());

  static int L();
  friend int m();
  
  X();
};


int X::h() { this; }
int X::h2() const { this; }
//ERROR(2): int X::h3() {}    // type mismatch

int X::L() {
  //ERROR(3): this;      // not a nonstatic member
}

//ERROR(4): int X::m() {}    // does not exist

X::X() {
  // does not have a 'this' parameter, but there *is*
  // a local variable called 'this'
  this;
}


