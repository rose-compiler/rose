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
#include <wchar.h>

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


//probably we should have variations of these too!
wchar_t func_wchar_t(wchar_t a)
{
    return a*a;
}

//
//The shorts
//
//two flavours: signed (the unqualified one is taken as signed), unsigned 
short func_short(short a)
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


    //this is not a valid char
    //wchar_t c4 = L'caaaaaaaaaaaaaaaaaaaaaaaaaaaaaa';
    //func_wchar_t(c4);

    wchar_t c6 = L'c';
    func_wchar_t(c6);

    //the following does not pass
    //wchar_t str1[100] = UL"aaaaaa";
    //wchar_t str1[] = L"aaaaaa";

    printf("Hello\n");
    return 0;
}
