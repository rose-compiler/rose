// in gcc, in C mode, a foo and a struct foo have nothing to do with
// one another
typedef char *foo;
struct foo *q;
struct foo { int x; };
//ERROR(1): struct foo { int x2; };    // duplicate

typedef char *foo2;
struct foo2 { int x; };

typedef char *foo3;
enum foo3 { SOMETHING };

// the test exhibits the bug without the below, but the below extends
// the test
int main() {
  struct foo *gronk = q;
  foo zork;
  char *bork = zork;
}

