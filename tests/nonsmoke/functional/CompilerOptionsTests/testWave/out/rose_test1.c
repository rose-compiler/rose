#include <stdio.h>
#include <stdbool.h>
extern bool obfuscated_condition();
volatile int input;

int fib(int n)
{
  int a = 1;
  int b = 1;
  bool (*cond)();
  cond = (obfuscated_condition);
  if (n == 0) 
    return 1;
  while((n > 1) && (((( *cond)())) != 0)){
    --n;
    int result = (a + b);
    a = b;
    a += input;
    b = result;
    a -= input;
  }
  return b;
}


int main()
{
  int x = input;
  int prod = 1;
  while(x > 0){
    prod *= x;
    --x;
  }
  printf(("factorial(input) = %d\n"),prod);
  prod = fib(input);
  printf(("fib(input) = %d\n"),prod);
  return 0;
}


bool obfuscated_condition()
{
  return 1;
}

