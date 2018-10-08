int f() {
  return 1;
}

int g(int a, int b, int c) {
  return 2+a+b+c;
}

int main() {
  int x,y,a,b;
  x=1;
  if(x>0) {
    a=f();
    b=a+f()+2;
  }
  else
    y=x+1;

  y=y+g(x+10,y+10,a+10);
  int* xp;
  int* yp;
  xp=&x;
  yp=&y;
  *xp=*yp;

  int A[2];
  A[0]=1;
  A[1]=A[0];
  int B[2][2];
  B[0][0]=1;
  B[1][1]=B[0][0]+1;
  return 0;
}
