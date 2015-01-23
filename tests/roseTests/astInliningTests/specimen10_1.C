struct w {
  int data;
  w(int data, int dog): data(data) {}
};

int foo() {
  return 7;
}

int main(int, char**) {
  w zzz(foo(), 9);
  return !(zzz.data == 7);
}
