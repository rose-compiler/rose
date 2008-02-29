// Test of designators in structs
struct S {
  int a[5];
  double b;
};

int main(int argc, char** argv) {
  struct S x = {
    b: 3.14,
    a: {
      [1] = 1,
      [3] = 2,
      3
    }
  };
  return 0;
}
