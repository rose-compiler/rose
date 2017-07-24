namespace namespace1 {
  template <class T> void foo(T t) {}
}

class className {
  public:
    void bar(int value) {
      namespace1::foo<int>(value);
    }
};
