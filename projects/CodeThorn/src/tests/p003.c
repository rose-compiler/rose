#include <stdio.h>

int x=1;

int main() {
  int input;
  int output;
  scanf("%d",&input);
  if(input==1 && x==1)
    output=1;
  else
    output=2;
  printf("%d",output);
  return 0;
}
