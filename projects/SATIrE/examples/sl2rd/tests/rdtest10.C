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
  for(int i=1;i<10;i=i-1) {
    for(int j=1;j<i;j=j-1) {
      x=x+j*i;
    }
    x=x-1;
  }
  return 0;
}
