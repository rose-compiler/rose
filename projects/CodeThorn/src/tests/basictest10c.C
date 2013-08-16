int main() {
  int x;
  int y;
  x=0;
  y=1;
  while(1) {
    y=2;
    while(x!=5) {
      if(x==1) {
        x=2;
        break;
      }
      x=1;
    }
    y=2;
  }
  x=4;
  return 0;
}
