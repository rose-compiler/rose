int main()
{
    int a,b,c;
    int *p,*q;
    a=10;
    p=&a;
    #pragma rose [pointerAliasLattice: p:Aliases=[a]]


    if(1)
    {
      p = &b;
      #pragma rose [pointerAliasLattice: p:Aliases=[b]]
    }
    else
      { 
        int *x;
        x = &c;
        #pragma rose [pointerAliasLattice: p:Aliases=[a], x:Aliases=[c]]
      }

    q = p;
    #pragma rose [pointerAliasLattice: p:Aliases=[a,b], q:Aliases=[a,b], x:Aliases=[c]]

    return 0;
}
