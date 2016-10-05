// Liao, 3/13/2012
// test cases for various function objects
// named, expression, and aliased, pointer to function
void foo (); 
void foo ()
{
  
}

void bar()
{
  // pointer to a function
  void (*func) (void) ;
  func = foo;
  foo();
  (*func)();
}
