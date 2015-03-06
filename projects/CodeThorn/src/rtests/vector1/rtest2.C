void func() {
  int x;
  int y;
  x=0;
  y=1;
  if(x==0 && y==1)
    y=y+1;
  while(x<10) {
    x=x+1;
    y=y+1;
    y=y+1;
    if(y==4) {
      y=y+1;
    }
  }
  int a[3];
  a[1]=2;
  a[2]=a[1];
  a[2]+=5;
  x=0;
  a[1]+=a[x++];
}

int main() {
  func();
  return 0;
}
