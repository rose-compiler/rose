//URK: 08/01/06
//This program tests the identity translator for special values
//The identity translator does not work on some of these special functions.
//
//A solution is to pass from EDG to SAGE.
//Yet to figure out a way to do it cleanly.
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

    //The float special values
    //**************************************************
#ifndef __INTEL_COMPILER
    float special_float_1 = __builtin_huge_valf();
    func_float(special_float_1);
#endif

    float special_float_2 =  __builtin_nanf("");
    func_float(special_float_2);

    float special_float_3 = __builtin_nansf("");
    func_float(special_float_3);


    //The double special values
    //**************************************************
#ifndef __INTEL_COMPILER
    double special_double_1 = __builtin_huge_val();
    func_double(special_double_1);
#endif

    double special_double_2 =  __builtin_nan("");
    func_double(special_double_2);

    double special_double_3 = __builtin_nans("");
    func_double(special_double_3);


    //The long long double special values
    //**************************************************
#ifndef __INTEL_COMPILER
    long double special_long_double_1 = __builtin_huge_vall();
    func_long_double(special_long_double_1);
#endif

    long double special_long_double_2 =  __builtin_nanl("");
    func_long_double(special_long_double_2);

    long double special_long_double_3 = __builtin_nansl("");
    func_long_double(special_long_double_3);


    printf("Hello\n");
    return 1;
}
