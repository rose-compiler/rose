/*
  This file is for testing neste for loops.
*/
int printf(const char *, ...);

int main(){

  float a;
  a=5;
  float b = 6;
  float c = 5;
  int i,j,k;
  i=1;
  j=0;

  b=3;
  float x=0;

  if(a<=b){
    j=5;
    i=0;
    a=j+5;
  }
  
  for(j=i; j<a; j=j+i){
    b--;   
    c--;
    int  d;
    for(k=2; k<j; k++){
      x+=x*i;
    }
    int r;
   b=c;
  }
  
  return 0;
}
