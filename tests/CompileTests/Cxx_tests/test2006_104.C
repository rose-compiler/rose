//URK: 08/01/06
//This program tests for the handling of float literals
//


#include <stdio.h>
float func_float(float a)
{
    return a*a;
}

double func_double(double a)
{
    return a*a;
}

double func_long_double(long double a)
{
    return a*a;
}

int main()
{
    //double
    float simple_float = 3.1415926;
    func_float(simple_float);

    //doubles
    double simple_double = +2.718281828;
    func_double(simple_double);

    //long doubles
    long double simple_long_double = -1.61803399;
    func_long_double(simple_long_double);

    //has to be float because of the f suffix
    float simple_float_sub = 3.1415926f;
    func_float(simple_float_sub);

    //has to be long double because of the suffix
    long double simple_long_double_sub = -1.61803399l;
    func_long_double(simple_long_double_sub);

    printf("Hello\n");
    return 1;
}
