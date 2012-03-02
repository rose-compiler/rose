int main() {
  int x,y,z,u;
  x=5;
  y=7;
  z=x+y*2;
  if(z<20) {
    z=z/2;
    if(z+x>x/2+y) {
      z=z-(x+y);
    }
  } else {
    z=z*2;
  }
  if(x>u) {
    x=y+2;
  } else {
    x=y-2;
  }
  y=y+1;
  
  return 0;
}
