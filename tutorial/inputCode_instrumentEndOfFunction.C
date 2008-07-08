/* Example code:
 *  a function with multiple returns
 *     some returns have expressions with side effects
 *  a function without any return   
 */ 
extern int foo();
extern int call1();
int main(int argc, char* argv[])
{
  if (argc>1)
    return foo();
  else
     return foo();
  return 0;
}

void bar()
{
  int i;  
}
