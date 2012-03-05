int d;
int inc(int x) {
  d=d+1;
  return x+1;
}

int main() {
  int a,b,c;
  a=3;
  b=a;
  d=1;
  while(a<10) {
    if(a<b) {
      a=a+1;
    } else {
      b=inc(b)+inc(b);
    }
    c=a+b;
  }
  d=d+c;
  return 0;
}
