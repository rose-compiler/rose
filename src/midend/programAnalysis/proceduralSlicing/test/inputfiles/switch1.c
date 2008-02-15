/*
  This file is for testing the switch-case structure.
*/
int printf(const char *, ...);

int main(){
  
  int color=5;
  int a,b;
  int c,d;
  int r;
  int r2;
  a=4;
  b=6;
  c=0;
  b--;
  r2 = a-b;
 
  switch(color){
  case 1:
    a--;
    c=b;
    break;
  case 2:
    b++;
    c=a;
    break;
  default:
    break;
  }
  
  r = a-b;

  return 0;
}
