// this form shows up in the kernel
int a[] = { [1] /*no = or : here*/ 0, [2] 10, [3] 13,};
struct A {
  int x;
  int y;
};
int main() {
  struct A a = { .y /*no = or : here*/ 3, .x 8 };
}
