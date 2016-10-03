class A {
  private:
  A(const A&);

  public:
  A() {}
  explicit A(int) {}
};

A a(5);

const A& foo() {
  return a;
}

int main(int, char**) {
  const A& b = foo();
  return 0;
}
