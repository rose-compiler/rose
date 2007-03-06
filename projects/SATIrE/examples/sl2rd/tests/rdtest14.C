int inc(int x) {
  return x+1;
}

int main() {
  int a,b,c;
  a=3;
  b=a;
  while(a<10) {
    if(a<b) {
      a=a+1;
    } else {
      b=inc(b)+inc(b);
    }
    c=a+b;
  }
  a=c;
  return 0;
}
