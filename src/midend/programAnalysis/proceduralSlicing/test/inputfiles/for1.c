/*
This file is for testing for loops and the test expression within the for loop construction.
*/
int printf(const char *, ...);

int main(){
  float b=7;
  int x, y=5;
  float a=8;
  int i;
  
  y=3;
  x=4;
  x = x+2;

  b=2;

    for(i=0;i<=y;i++){
    b=b+x*x;
    x=x+1;
    a=a+x;
    b=a;  
  }
  int j;
  for(j=1; j<=y;j++){
    x--;
    }
  return 0;
}
