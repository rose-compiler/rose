// Program, based on example in Knoop et al ("Optimal code motion: theory and
// practice", ACM TOPLAS 16(4), 1994, pp. 1117-1155, as cited in Paleri et al
// (see pre.C)), converted to C++

// DQ (12/16/2006): This appears to be OK now (it was a previous ROSE bug).
// int unknown(); // ROSE bug: including body "return 0;" here doesn't work
int unknown() { return 0; }

void foo() {
  int a, b, c, x, y, z, w;

  if (unknown()) {
    y = a + b;
    a = c;
    // Added by Jeremiah Willcock to test local PRE
    w = a + b;
    a = b;
    x = a + b;
    w = a + b;
    a = c;
    // End of added part
    x = a + b;
  }

  if (unknown()) {
    while (unknown()) {y = a + b;}
  } else if (unknown()) {
    while (unknown()) {}
    if (unknown()) {y = a + b;} else {goto L9;}
  } else {
    goto L10;
  }

  z = a + b;
  a = c;

  L9: x = a + b;

// DQ (12/16/2006): This appears to be OK now (it was a previous ROSE bug).
// L10: 0; // ROSE bug: using return; here doesn't work
  L10: return; // ROSE bug: using return; here doesn't work
}

int main(int, char**) {
  foo();
  return 0;
}
