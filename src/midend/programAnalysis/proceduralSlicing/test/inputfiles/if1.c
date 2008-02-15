/*
  This file is for testing if-tests.
*/
int printf(const char *, ...);
                                                                               
int main(){
  
  float b=7;
  int x, y=5;
  float a=8;
  int i;
  y=9;
  x=4;

  x++;
  
  if(a<=y){
    b=b+x*x;
    x = x+1;
    a=a+x;  
  }
  x+=5;

  for(i=1; i<=y;i++)
    x--;
  
  return 0;
}
 
