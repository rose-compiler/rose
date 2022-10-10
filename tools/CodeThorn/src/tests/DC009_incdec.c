int main() {
  int x=1;
  int y1=0,y2=0;
  y1=x++;
  int z=0;
  if(y1==1) {
    z+=1;
  } else {
    z+=2;
  }
  y2=++x;
  if(y2!=2) {
    z+=4;
  }
  return 0;
}
  

