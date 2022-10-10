int main() {
  int x=2;
  int r=0;
  switch(x) {
    int y;
  case 0:
  case 1: r=1;
  case 2: r=y;
    break;
  case 3: r=3;
    break;
  case 0xA0: {
    r=4;
    break;
  }
  default: 
    r=100;
    r=r+1;
    break;
  case 0xB0:
    r=5;
    r=r+1;
  }
  return !(r==r);
}
