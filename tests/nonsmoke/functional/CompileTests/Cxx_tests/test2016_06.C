#if 0
class A {
  A();
  A(const A& x);
  A(int x);
  A& operator=(const A& x);
  int x;
  int y;
};

class A2 {
  A2();
  A2(const A2& x) {}
  A2(int x) {}
  A2& operator=(const A2& x) {}
  int x;
  int y;
};

class B {
  //B();
  //  B(const A& x);
  B(int x);
  //B& operator=(const A& x);
  int x;
  int y;
};
#endif

#if 0
template<typename T>
class C {
  C() {}
  C(const C& x) {}
  C(int x) {}
  C& operator=(const C& x) {}
  int x;
  int y;
};
#endif

template<typename T>
class C {
  // C* pointer_variable;
  C(const C& x);
  // C(const C& x) {}
  // C& operator=(const C& x) {}
};


// int main() {}
