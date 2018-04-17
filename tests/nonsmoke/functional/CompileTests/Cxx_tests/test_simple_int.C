#include <stdio.h>
char func_char(char a)
{
    return a+2;
}

short func_short(short a)
{
    return a+2;
}

int func_int(int a)
{
    return a+2;
}

long func_long(long a)
{
    return a+2;
}

int main()
{
    //char
    char simple_char = '3';
    func_char(simple_char);


    //short
    short simple_short = 3;
    func_short(simple_short);


    //long
    long simple_long = 3L;
    func_short(simple_long);

    //long
    long simple_long12 = 03L;
    func_short(simple_long12);





    printf("Hello\n");
    return 1;
}
