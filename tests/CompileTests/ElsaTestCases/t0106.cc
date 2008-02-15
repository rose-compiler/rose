// demonstrates that you can't use the bool cond ( (whatever)) syntax
// for constructing a variable inside a while condition
int main() {
  int whatever = 3;
  int whoever(7);
  int whichever[] = {1, 2, 3};
  while (whatever) 14;
  while (bool cond = whatever) 15;
}
