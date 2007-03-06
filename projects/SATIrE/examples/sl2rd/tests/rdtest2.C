int main() {
  int a,b,c;
  a=3;
  b=a;
  if(a<10) {
    if(a<b) {
      a=a+1;
    } else {
      b=b+1;
    }
    c=a+b;
  }
  while(c>0) {
    c=c-1;
  }
  c=c/2;
  return 0;
}
