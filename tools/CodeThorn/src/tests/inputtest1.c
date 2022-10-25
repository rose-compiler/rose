#include <stdio.h>

int f(int input2) {
  if(input2==3)
    return 1;
  if(input2==5)
    return 2;
  if(input2==10||input2==11)
    return 3;
  if(input2==12&&input2==13)
    return 4; // unreachable
  if(!(input2==14))
    return 5;
  return 100;
}

int main() {
  int input,output;
  while(1) {
    scanf("%d",&input);
    output=f(input);
    printf("%d",output);
  }

  return 0;
}
