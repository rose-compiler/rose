
// calls: main: ->f3, ->f2->f3, ->f1->f2->f3
// functions f4 and f5 are unreachable, f4 calls f5 and this should not interfer with result
// tests for callreturn edge from f3 to f4 and context sensitivity

void f3();

// dead
void f5() {
  int e;
  f5();
}

// dead
void f4() {
  int d;
  f3(); // calling an analyzed function from dead code
  f5();
}

void f3() {
  int c;
}

void f2() {
  int b;
  f3();
}

void f1() {
  int a;
  f2();
}


int main() {
  f3();
  f2();
  f1();
}
