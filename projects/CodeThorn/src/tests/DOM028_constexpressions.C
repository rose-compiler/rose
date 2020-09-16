void foo(int x) {
}

int main() {
  int x1=1;
  int x2=1+1;
  int x3=1+1+1;
  int x4=8/4*2;
  int x5=8/4*2+1;
  int x6=5 % 3;
  int x7=-200;
  int x8=-200*5/2+(4*2);
  int x9=0?1:2;
  int x10=x1?1:2;
  bool b1=true || false;
  bool b2=1 && 1;
  bool b3=0 && x1;
  bool b4=10 || x1;

  int a[2]={100,101};
  int x11=a[0];
  int x12=1+a[0];
  int x13=1+a[0];

  struct S { int a; int b; };
  S s={200,201};
  int x14=s.a;
  int x15=1+s.a;
  S* s2=new S();
  delete s2;
}
  
