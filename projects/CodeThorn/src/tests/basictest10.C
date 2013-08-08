int main() {
  int x;
  int y;
  x=0;
  y=1;
  while(x!=5) {
    if(x==2 && y==2)
      break;
    if(x==2 && y==1)
      y=2;
    if(x==1 && y==1)
      x=2;
    if(x==0)
      x=1;
  }
  x=3;
  x=x+1;
  return 0;
}
