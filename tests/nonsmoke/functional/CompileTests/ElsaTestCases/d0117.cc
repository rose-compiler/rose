// operator() was not cloning the type when it set the type of the
// function variable when replacing the operator with a normal
// function call

template<class T> struct R {
  T *get() {}
};

struct S {
  int operator() (int *);
};

int f() {
  S a;
  R<int> i;
  a(i.get());
}
