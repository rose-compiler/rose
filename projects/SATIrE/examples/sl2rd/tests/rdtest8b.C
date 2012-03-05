int inc(int v) {
  int r=v+1;
  return r;
}

int main() {
  int x,r1,r2;
  x=5;
  r1=inc(x);
  while(r1<10) {
    int y=2;
    r1=r1+1+y;
    return 0;
  }
  return 0;
}
