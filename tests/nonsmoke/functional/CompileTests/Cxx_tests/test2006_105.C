//URK: 08/01/06
//This program tests for looong values of floats, doubles and long doubles.
//
//In EDG, the float_value_lexeme element of a_constant is now a character array 
//of length 128. Once it is made into a string, more test cases needed to be added.

#include <stdio.h>
float func_float(float a)
{
    return a*a;
}


double func_double(double a)
{
    return a*a;
}

long double func_long_double(long double a)
{
    return a*a;
}

int main()
{
    //******************************************
    //doubles
    //These are loooong floats with and without exponents
    float looong_simple_float = 1.111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111;
    func_float(looong_simple_float);

    float looong_simple_float_exp = 1.21111111111111111111111111111111111111111111111111111111111111111111111111111e11;
    func_float(looong_simple_float_exp);


    //These are loooong floats with and without exponents. 
    //These *have* to be floats (in internal representation), because of the subscripts f
    float looong_simple_float_subscript = 1.311111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111f;
    func_float(looong_simple_float_subscript);

    float looong_simple_float_exp_subscript = 1.411111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111e11f;
    func_float(looong_simple_float_exp_subscript);



    //******************************************
    //doubles
    //These are loooong doubles with and without exponents
    double looong_simple_double = 2.122222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222;
    func_double(looong_simple_double);

    double looong_simple_double_exp = 2.22222222222222222222222222222222222222222222222222222222222222222222222222222e122;
    func_double(looong_simple_double_exp);


    //These are loooong doubles with and without exponents. 
    //These *have* to be doubles (in internal representation), because of no subscript
    double looong_simple_double_subscript = 2.322222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222f;
    func_double(looong_simple_double_subscript);

    double looong_simple_double_exp_subscript = 2.422222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222e12f;
    func_double(looong_simple_double_exp_subscript);


    //loong long doubles
    //These are loooong long doubles with and without exponents
    long double looong_simple_long_double = 3.133333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333;
    func_long_double(looong_simple_long_double);

    long double looong_simple_long_double_exp = 3.23333333333333333333333333333333333333333333333333333333333333333333333333333e133;
    func_long_double(looong_simple_long_double_exp);


    //These are loooong long doubles with and without exponents. 
    //These *have* to be long doubles (in internal representation), because of the subscripts l
    ////lookout for the L or l at the end
    long double looong_simple_long_double_subscript = 3.333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333L;
    func_long_double(looong_simple_long_double_subscript);

    long double looong_simple_long_double_exp_subscript = 3.433333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333e133L;
    func_long_double(looong_simple_long_double_exp_subscript);


    printf("Hello\n");
    return 1;
}
