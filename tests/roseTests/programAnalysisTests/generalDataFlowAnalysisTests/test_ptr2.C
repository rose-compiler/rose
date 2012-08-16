int a;

void foo(int* &x)        
{
    int b  = 10;    
    x = &b;
    #pragma rose x:Aliases:{b}{}b:Aliases:{}{}

}

void main()
{
    int *p;
    a = 20;
    foo(p);
    #pragma rose a:Aliases:{}{}p:Aliases:{b}{}
}
