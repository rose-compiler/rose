int a;


void main()
{
    int **p;
    int *x;

    a = 20;

    x = &a;
    #pragma rose [pointerAliasLattice: x:Aliases=[a]]
    
    p = &x;
    #pragma rose [pointerAliasLattice: p:Aliases=[x], x:Aliases=[a]]

    int b=0;
    *p = &b;
    #pragma rose [pointerAliasLattice: p:Aliases=[x],x:Aliases=[b]]

}
