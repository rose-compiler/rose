int fib(int z, int u) {
  int v,r;
  if(z<3) {
    v=u+1;
  } else {
    v=fib(z-1,u);
    v=fib(z-2,v);
  }
}
  
int main() {
  int x,y;
  x=5;
  y=fib(x,0);
  return 0;
}
