class A {
  private:
  A();

  public:
  explicit A(int) {}
};

A foo() {
  return A(5);
}

int main(int, char**) {
  A x = foo();
  return 0;
}
