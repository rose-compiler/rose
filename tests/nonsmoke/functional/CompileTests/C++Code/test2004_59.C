/*
Hi Dan,
If you run the Identity Translator with the input file I have attached, 
the program goes into a infinite cycle.

It seems to me that a class (or struct) instantiation in conjunction 
with pragmas(or pragma)  creates this situation. If you take either of 
them out, it works correctly.

I thought I might want to know about this bug.

p.s. Jeremiah tried it on his Identity translator and it failed too.

-- 
Alin Jula
*/

class A{
};

int main()
{
A a;
#pragma foo
#pragma bar
}

