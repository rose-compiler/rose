int main() {
  int i = 1;
  i = 2; // not a dead store!
  i += 3;
}
