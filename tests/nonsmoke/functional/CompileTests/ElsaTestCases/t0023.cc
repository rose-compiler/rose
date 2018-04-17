// cc.in23
// exception specs on functions

int foo() throw();

int foo() throw()
{
  return 3;
}


// 15.4 para 1: this must be a complete type
class Exc {};

void bar() throw(Exc);

//ERROR(1): void bar();    // conflicting declaration

