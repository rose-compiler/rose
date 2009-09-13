//URK: 08/22/06
//This program tests for different flavours of ints
//refer to http://home.att.net/~jackklein/c/inttypes.html and Stroustrup's books
//
//In EDG, the float_value_lexeme element of a_constant is now a character array 
//of length 128. Once it is made into a string, more test cases needed to be added.
//
//
//Also look at 
//http://osr5doc.ca.caldera.com:457/topics/ComplStdC_LangFeatures.html
//for notes on character and string literals.
//
//The notes in the tentative proposal
//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1823.html
//is kind of scary! It says that even u'a', UL'a' and a whole slew of other stuff could be 
//character literals (and hence string literals) when the proposal is accepted.

#include <stdio.h>

//
//The chars
//
//three flavours: char, signed char and unsigned char
char func_char(char a)
{
    return a*a;
}

signed char func_signed_char(signed char a)
{
    return a*a;
}

unsigned char func_unsigned_char(unsigned char a)
{
    return a*a;
}


/*
//probably we should have variations of these too!
wchar_t func_wchar_t(wchar_t a)
{
    return a*a;
}
*/

//
//The shorts
//
//two flavours: signed (the unqualified one is taken as signed), unsigned 
short func_short(short a)
{
    return a*a;
}

unsigned short func_unsigned_short(unsigned short a)
{
    return a*a;
}

//The ints
int func_int(int a)
{
    return a*a;
}

unsigned int func_unsigned_int(unsigned int a)
{
    return a*a;
}

//The longs
long func_long(long a)
{
    return a*a;
}

unsigned long func_unsigned_long(unsigned long a)
{
    return a*a;
}

//The long longs
long long func_long_long(long long a)
{
    return a*a;
}


unsigned long long func_unsigned_long_long(unsigned long long a)
{
    return a*a;
}



int main()
{
    //******************************************
    //ints

    //the chars
    char c1 = 'c';
    func_char(c1);

    //the long/"wide" chars
    char c2 = L'c';
    func_char(c2);


    //gcc allows initializations of the following kind silently
    //EDG does not.
    //char c3 = 'caaaaaaaaaaaaaaaaaaaaaaaaaaaaaa';
    //func_char(c3);

    //this is not a valid char
    //wchar_t c4 = L'caaaaaaaaaaaaaaaaaaaaaaaaaaaaaa';
    //func_char(c4);

    wchar_t c6 = L'c';
    func_char(c6);

    //the following does not pass
    //wchar_t str1[100] = UL"aaaaaa";
    //wchar_t str1[] = L"aaaaaa";


    //shorts
    //I donot know if there is any short-integer-literal.
    short simple_short = 1234L;
    func_short(simple_short);

    //The ints
    int simple_int = 1234;
    func_int(simple_int);

    unsigned int simple_Uint = 1234U;
    func_unsigned_int(simple_Uint);

    unsigned int simple_uint = 1234u;
    func_unsigned_int(simple_uint);

    //The longs
    //the signed ones
    long simple_long1 = 1234L;
    func_long(simple_long1);

    long simple_long2 = 1234l;
    func_long(simple_long2);

    //the unsigned ones
    unsigned long simple_unsigned_long1 = 1234ul;
    func_long(simple_unsigned_long1);

    unsigned long simple_unsigned_long2 = 1234Ul;
    func_long(simple_unsigned_long2);

    unsigned long simple_unsigned_long3 = 1234uL;
    func_long(simple_unsigned_long3);

    unsigned long simple_unsigned_long4 = 1234UL;
    func_long(simple_unsigned_long4);

    unsigned long simple_unsigned_long5 = 1234lu;
    func_long(simple_unsigned_long5);

    unsigned long simple_unsigned_long6 = 1234lU;
    func_long(simple_unsigned_long6);

    unsigned long simple_unsigned_long7 = 1234Lu;
    func_long(simple_unsigned_long7);

    unsigned long simple_unsigned_long8 = 1234LU;
    func_long(simple_unsigned_long8);

    //The long longs
    //the signed ones
    long long simple_long_long1 = 1234ll;
    func_long(simple_long_long1);

    long long simple_long_long2 = 1234LL;
    func_long(simple_long_long2);

    //the unsigned ones
    unsigned long long simple_unsigned_long_long1 = 1234ull;
    func_unsigned_long_long(simple_unsigned_long_long1);

    unsigned long long simple_unsigned_long_long2 = 1234Ull;
    func_unsigned_long_long(simple_unsigned_long_long2);

    unsigned long long simple_unsigned_long_long3 = 1234uLL;
    func_unsigned_long_long(simple_unsigned_long_long3);

    unsigned long long simple_unsigned_long_long4 = 1234ULL;
    func_unsigned_long_long(simple_unsigned_long_long4);

    unsigned long long simple_unsigned_long_long5 = 1234llu;
    func_unsigned_long_long(simple_unsigned_long_long5);

    unsigned long long simple_unsigned_long_long6 = 1234llU;
    func_unsigned_long_long(simple_unsigned_long_long6);

    unsigned long long simple_unsigned_long_long7 = 1234LLu;
    func_unsigned_long_long(simple_unsigned_long_long7);

    unsigned long long simple_unsigned_long_long8 = 1234LLU;
    func_unsigned_long_long(simple_unsigned_long_long8);

    printf("Hello\n");
    return 0;
}
