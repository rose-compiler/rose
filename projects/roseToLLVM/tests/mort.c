#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    double p, yr, r, n, power, amount;

    printf("Enter mortgage amount: ");
    scanf("%lf", &p);

    printf("Enter interest rate: ");
    scanf("%lf", &yr);

    printf("Enter number of years: ");
    scanf("%lf", &n);
    n *= 12.0;

    printf("\n\n");

    r = yr / 1200.0;
    power = exp(n * log(1.0 + r)); /* same as: (1.0 + r) ** n */
    amount = p * ((r * power) / (power - 1.0));

    printf("\n");
    printf("Total amount of loan: $%.2lf\n", p);
    printf("Yearly interest rate: %.4lf%%\n", yr);
    printf("Length of loan: %.2lf Year(s)\n", n / 12);
    printf("Fixed monthly amount: $%.2lf\n", amount);

    return 0;
}
