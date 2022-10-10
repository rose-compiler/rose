int main() {
  int a;
  int b=a;
  int c=1;
  int d=c;
  int e=b+c+d;
  int f=d;
  a=c;
  e=d;
  return e>42;
}
