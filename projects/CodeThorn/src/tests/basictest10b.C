int main() {
  int x;
  int y;
  x=0;
  y=1;
  while(x!=5) {
    if(x==1) {
      x=2;
      break;
    }
    x=1;
  }
  while(x!=3) {
    if(x==1) {
      x=6;
      break;
    }
    x=3;
  }
  x=4;
  return 0;
}
