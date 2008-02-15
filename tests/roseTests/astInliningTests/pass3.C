int foo() {
  return 7;
}

int main(int, char**) {
  if (foo() == 9)
    return 1;
  else
    return 0;
}
