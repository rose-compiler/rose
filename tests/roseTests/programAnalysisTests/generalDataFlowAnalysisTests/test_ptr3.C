int *p;
int x,a,b;

void foo()
{
  x=10;
  a=10;
  int c = b;
  p = &x;
  #pragma rose [pointerAliasLattice: p:Aliases=[x]]
}

void main()
{
  x = 79;
  a=20;
  int *r = &a;
  #pragma rose [pointerAliasLattice: SgAssignInitializer:Aliases=[a]]


  b = 8;

  foo();
  #pragma rose [pointerAliasLattice: p:Aliases=[x], SgAssignInitializer:Aliases=[a]]
  
  x  = b + a + x;

  p = &b;
  #pragma rose [pointerAliasLattice: p:Aliases=[b], SgAssignInitializer:Aliases=[a]]
}

