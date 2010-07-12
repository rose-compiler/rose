#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
  double p = 10000, // principal
           yr = 6,  // yearly rate
           r,
           n = 4,   // number of years
           power,
           amount;

    n *= 12.0;

    r = yr / 1200.0;
    power = exp(n * log(1.0 + r)); /* same as: (1.0 + r) ** n */
    amount = p * ((r * power) / (power - 1.0));

    printf("Fixed monthly amount: $%.2lf\n", amount);

    return 0;
}
