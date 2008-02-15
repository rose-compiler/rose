// cc.in80
// reproduce ASTTypeId ambiguity

int f(int);
int g(int);

int func(int param)
{
  // unambiguous
  //int y(f(param));
  
  // ambiguous
  int x(f(g(param)));
}


// different way of seeing it
typedef int x;
typedef int y;
typedef int z;

int h(int /*anon*/(x /*anon*/(y /*anon*/)));
    
