void foo1();

void foo1() {
  int x;
  if(x==x) {
    x=10;
  } else {
    x=300;
  }
  {
    int y;
    int c=4;
    int x2=x;
    y=(x2-c)*8;
    int a[1024];
    int res;
    res=a[y];
  }
}

int main() {
  foo1();
}
