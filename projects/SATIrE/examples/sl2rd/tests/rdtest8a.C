int inc(int v) {
  int r=v+1;
  return r;
}

int main() {
  int x,r1,r2;
  x=5;
  r1=inc(x);
  x=3;
  r2=inc(x);
  if(r1+r2==10)
    return 0;
  else
    return 1;
}
