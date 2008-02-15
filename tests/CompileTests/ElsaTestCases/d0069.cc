// test function template overloading

template <class T> struct A {};

int f(A<int>&) {}
int *f(int) {}

int main() {
  int *x = f(3);
}
