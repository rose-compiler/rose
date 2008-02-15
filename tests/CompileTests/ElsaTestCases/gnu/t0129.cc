// test compound literals
// http://gcc.gnu.org/onlinedocs/gcc-3.2.2/gcc/Compound-Literals.html#Compound%20Literals
int main() {
  int q = 7;
  struct foo {
    int x;
    int y;
  } f;
  f = (struct foo) {q, 4};
}
