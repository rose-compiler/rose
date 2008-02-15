/*
This file is for testing while loops.
*/
int printf(const char *, ...);

int main(){
  float i = 5;
  float prod = 1;
  float sum = 0;
  while(i<10){
    sum+=i;
    prod*=i;
    i++;
  }
  sum; 
  prod; 

  return 0;
}
