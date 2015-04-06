int main() {
  int x=1;
  int y=2;
  int z1=0;
  int z2=z1;
  int z3=z2;
  int z4;
  z4=1;
  if(true) {
    z1++;
    z2++;
    z3++;
  }
  else {
    ++z1;
    ++z2;
    ++z2;
    z3=1;
  }
  x=2;
  while(true) {
    ++z4;
  }
  x=y=3;
  return 0;
}
