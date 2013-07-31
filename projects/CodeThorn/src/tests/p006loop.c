/*

3 -> 6
7 -> 7
x -> 5 when x!=3 and x!=7
 
 */
#include <stdio.h>

int x=1;
int a=1;
int b=1;
int c=1;

int f(int input2) {
  int y;
  if(x==1 || input2==2)
    a=1;
  else
    a=2;
  if(x!=1 || input2==3)
    b=3;
  else
    b=4;
  if(input2!=3||input2==2)
    c=5;
  else
    c=6;
  return c;
}

int main() {
  int input;
  int output;
  while(1) {
    scanf("%d",&input);
    if(input!=7)
      output=f(input);
    else
      output=7;
    printf("%d",output);
  }
  return 0;
}
