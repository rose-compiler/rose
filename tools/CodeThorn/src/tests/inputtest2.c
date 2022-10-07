#include <stdio.h>

int f(int input2) {
  if(input2==12&&input2==13)
    return 4;
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
