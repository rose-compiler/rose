// Examples reported by Jeremiah Willcock

// Demonstration of mutable keyword (works fine in ROSE now).
struct foo {
  int a;
  char b;
  mutable bool c;
};

// This currently fails for some reason in MSTL
int foo() {
  int x;
  int y;
  y = 0;
  switch (x) {
    case 0: y = 3;
    l:
    case 1: y = y + 1; break;
    default: return 7;
    case 10: y = 5; goto l;
    case 11:
    while (true) {
      case 7: y = y - 1; // continue;
      case 8: return y;
    }
  }
  return 5;
}
