int main() {
  int x=2;
  int r=0;
  switch(x) {
  case 1: r=1;
  case 2: r=2;
    break;
  case 3: 
  jmptarget1:
    r=3;
    break;
  case 0xA0: {
    r=4;
    break;
  }
  default: r=100;
    break;
  case 0x60:
    r=5;
  }
  if(r==100) 
    goto jmptarget1;
  return !(r==r);
}
