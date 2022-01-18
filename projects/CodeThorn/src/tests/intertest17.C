int x=1;

void f5() {
  x=x+10;
}

void f4() {
  x=2;
}

void f3() {
  f4();
}

void f2() {
  f3();
}

void f1() {
  f2();
}


int main() {
  f1();
  f4();
  f5();
}
