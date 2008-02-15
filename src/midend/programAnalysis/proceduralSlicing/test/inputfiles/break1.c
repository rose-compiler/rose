/*
This file is for testing use of break statement.
*/
int printf(const char *, ...);

int main(){
  
  int a,b,c,d;
  float x,y,z;
  a=5;
  b=3;
  c=5;
  d=5;

  x=5;
  y=5;
  z=5;

  while(x<=y){
    x++;
    z=y-x;
    
    if(a==b){
      x--;
      y++;
      z=x+y;
      break;
    }
#pragma start
    b=d;
#pragma end
    a=b-c;
  }
  a;
  
  return 0;
}
