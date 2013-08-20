#include <stdio.h>

int x=1;

int f(int input2) {
  return input2;
}

int main() {
  int input;
  int output1,output2;
  scanf("%d",&input);
  // first argument is false, due to short-circuit eval is !(input==1) not evaluated
  if(!(x==1) && !(input==1))
    output1=1;
  else
    output1=2;
  output2=f(output1);
  printf("%d",output2);
  return 0;
}
