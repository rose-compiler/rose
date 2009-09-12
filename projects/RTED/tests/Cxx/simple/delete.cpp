
int* foo() {
  int* x;
  int* y;

  x = new int;
  y = new int;

  delete x;
  return y;
  // so far, no error
}

int main() {

  {
    int* p;
    p = foo();
    // error, p goes out of scope, but the int on the heap was not freed
  }

  return 0;
}
