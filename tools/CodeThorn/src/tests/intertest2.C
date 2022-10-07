int f1(int x) {
  if(x==1) {
    return x;
  } else {
    return x+1;
  }
}
int f2() {
  return 2;
}

int main() {
  int x=1;
  f1(x);
  f2();
  return 0;
}
