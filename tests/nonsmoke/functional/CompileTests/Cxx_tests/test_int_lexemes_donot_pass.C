//URK: 08/22/06
//This program tests for different flavours of ints
//
//According to the standard, the suffixes 
//lL and Ll donot represent long long.
//Only LL and ll do.

#include <stdio.h>

long long func_long_long(long long a)
{
    return (a*a);
}


int main()
{
    //******************************************
    //ints


    /*
    long long simple_long = 1234Ll;
    func_long_long(simple_long);

    long long simple_long1 = 1234lL;
    func_long_long(simple_long1);
    */

    //char c3 = l'c';
    //the following kind is not allowed
    //neither in gcc nor in EDG
    //char c3 = l'c';

    printf("Hello\n");
    return 0;
}
