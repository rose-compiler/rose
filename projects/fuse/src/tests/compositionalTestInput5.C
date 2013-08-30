// This test evaluates the ability of 1-level call context sensitivity to disambiguate the possible states 
// inside a function based on the call sites. In this example we need context sensitivity to realize
// that the only possible instance of foo() is one called inside main and since in this instance a==1,
// we can ignore its else branch, which contains other calls to foo().
#pragma fuse lc(ccs, ccs, cp, dp, cp)
void CompDebugAssert(bool);
int global;

int inc(int x)
{ return x+1; }

int foo(int a)
{
  if(a<=1) return 1;
  else return foo(100) + foo(200);
}

int main(int argc, char** argv)
{
  int a=1;

  CompDebugAssert(inc(a)==2);
  CompDebugAssert(foo(a)==1);
}


