// 8.2.7a.cc

class C {};
void f(int (C)) {}           // void f(int (*fp)(C c)) {}
                             // not: void f(int C);

int g(C);

void foo() {
  //ERROR(1): f(1);          // error: cannot convert 1 to function pointer
  f(g);                      // OK
}

