#include <stdio.h>
#include <stdlib.h>

int int_calc(int a, int b)
{
  int result;
  result = (a * b / 345 + 27) % 33;
  return result;
}

float float_calc(float a, float b)
{
  float result;
  result = (a * 0.762 + b / 73.2) * 129.2;
  return result;
}

int main(int argc, char *argv[])
{
  int a, b, result1;
  float c, d, result2;
  
  printf("hello\n");
  if (argc < 5) {
    printf("This program requires two integer and two floating-point arguments.\n");
    exit(-1);
  }
  sscanf(argv[1], "%d", &a);
  sscanf(argv[2], "%d", &b);
  sscanf(argv[3], "%f", &c);
  sscanf(argv[4], "%f", &d);

  result1 = int_calc(a, b);
  result2 = float_calc(c, d);
  printf("result1 = %d, result2 = %f\n", result1, result2);

  return 0;
}
