int foo() {
  return 7;
}

int main(int, char**) {
  do {
    return 0;
  } while (foo() == 9);
  return 0;
}
