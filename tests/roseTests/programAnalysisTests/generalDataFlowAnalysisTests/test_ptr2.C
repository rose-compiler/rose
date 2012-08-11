int a;

void foo(int* &x)        
{
    int b  = 10;    
    x = &b;
    #pragma rose [pointerAliasLattice: x:Aliases=[b]]

}

void main()
{
    int *p;
    a = 20;
    foo(p);
    #pragma rose [pointerAliasLattice: p:Aliases=[b]]
}
