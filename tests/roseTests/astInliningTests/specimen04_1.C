int foo() {
  return 7;
}

int main(int, char**) {
  while (foo() == 9)
    return 1;
  return 0;
}
