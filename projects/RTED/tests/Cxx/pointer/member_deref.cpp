class Test {

  public:
    void foo() {
      p = new int;
      *p = 42;

      // illegal write
      *((int*) *p) = 42;
    }

  private:
    int* p;
};


int main() {

  Test t;
  t.foo();

  return 0;
}
