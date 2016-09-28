// test combo designated initializers and compound literals
// Designated Initializers:
// http://gcc.gnu.org/onlinedocs/gcc-3.2.2/gcc/Designated-Inits.html#Designated%20Inits
int main() {
  struct foo {
    int x;
    double y;
  };
  struct foo f = (struct foo) {1, 2.3};
  struct goo {
    int x;
    double y;
  } g = {x:1, y:2.3};
  struct boo {
    int x;
    double y;
  };
  struct boo b = (struct boo) {x:1, y:2.3};
  struct oink {
    struct boo b;
    int i;
  };
  struct oink o = (struct oink) {i:7, b:{x:8, y:9.8}};
  return 0;
}
