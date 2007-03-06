int inc(int v) {
  return v+1;
}

int main() {
  int x,r1;
  x=5;
  r1=inc(x);
  x=3;
  r1=r1+inc(x);
  x=7;
  r1=r1+inc(x);
  return 0;
}
