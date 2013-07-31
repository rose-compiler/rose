int main() {
  int x;
  int y;
  x=1;
  y=0;
  if(true)
    y=1;
  if(100)
    y=1;
  if(x==1) {
  pos1: y=2;
  }
  if(x!=1) {
  pos2: y=3;
  }
  (x==1)? y=1: y=2;
  return 0;
}
