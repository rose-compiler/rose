int main() {
  int a,b,c;
  a=3;
  b=a;
  while(a<10 || b<10) {
    if(a<b) {
      a=a+1;
    } else {
      b=b+1;
    }
    c=a+b;
  }
  return c-c;
}
