class Animal{
private:
    int type;
public:
    void myType();
};

class Dog : public Animal{
private:
    int type;
public:
    void change();

};


int *p,*k;
int g=0;

#pragma rose [pointerAliasLattice: p:Aliases=[m]]
int bar(int flag)
{
  g = flag;
  int a =1; //b,c=2,x,y;
  int x,y,b,c;
  int *q;
  q=&b;// *r = &c;
  #pragma rose [pointerAliasLattice: p:Aliases=[m], q:Aliases=[b]]
  int *w;
  w = p;
  p=&g;
  #pragma rose [pointerAliasLattice: p:Aliases=[g], q:Aliases=[b]]
  
  k = &g;
  #pragma rose [pointerAliasLattice: p:Aliases=[g], q:Aliases=[b], k:Aliases=[g]] 
  
  q=&c;
  #pragma rose [pointerAliasLattice: p:Aliases=[g], q:Aliases=[c], k:Aliases=[g]] 
  
  p=q;
  #pragma rose [pointerAliasLattice: p:Aliases=[c], q:Aliases=[c], k:Aliases=[g]]

  q=p;
  #pragma rose [pointerAliasLattice: p:Aliases=[c], q:Aliases=[c], k:Aliases=[g]]

  if (flag == 0) // flag is only read here, not written! 
  {
    q = &x;
    #pragma rose [pointerAliasLattice: p:Aliases=[c], q:Aliases=[x], k:Aliases=[g]]
  }
  else  
  { int *p ;
    p = &y;
    #pragma rose [pointerAliasLattice: p:Aliases=[c], q:Aliases=[c], k:Aliases=[g], p:Aliases=[y]]
  }

  #pragma rose [pointerAliasLattice: p:Aliases=[c], q:Aliases=[x,c], k:Aliases=[g], p:Aliases=[y]]

  return c;  
}


int main()
{
 int m,n;
 int *r;
 g = 8;
 r = &g;
 #pragma rose [pointerAliasLattice: r:Aliases=[g]]
 
 Animal *an = new Animal();
 #pragma rose [pointerAliasLattice: r:Aliases=[g], an:Aliases=[_expression_xxx0_SgNewExp]]

 an = new Dog();
 #pragma rose [pointerAliasLattice: r:Aliases=[g], an:Aliases=[_expression_xxx1_SgNewExp]]

 k = k+1;
 #pragma rose [pointerAliasLattice: r:Aliases=[g], an:Aliases=[_expression_xxx1_SgNewExp]]

 p = &m;
 #pragma rose [pointerAliasLattice: p:Aliases=[m], r:Aliases=[g], an:Aliases=[_expression_xxx1_SgNewExp]]
 bar(3);
 #pragma rose [pointerAliasLattice: p:Aliases=[c], k:Aliases=[g], r:Aliases=[g], an:Aliases=[_expression_xxx1_SgNewExp]]
 p=&n;
 #pragma rose [pointerAliasLattice: p:Aliases=[n], k:Aliases=[g], r:Aliases=[g], an:Aliases=[_expression_xxx1_SgNewExp]]
 
 return 0;
}

