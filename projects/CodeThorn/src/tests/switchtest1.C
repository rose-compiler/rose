int main() {
  int x=2;
  int r=0;
  switch(x) {
  case 1: r=1;
  case 2: r=2;
    break;
  case 3: r=3;
    break;
  case 0xA0: {
    r=4;
    break;
  }
  default: r=100;
    break;
  case 0xB0:
    r=5;
  }
  return 0;
}
