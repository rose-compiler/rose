// cc.in78
// clash between implicit typedef and class member

class Foo {
  struct hash {
    int x;
  } hash;
};

void func()
{
  Foo f; 
  int i;
  
  i = f.hash.x;    // make sure 'hash' is a variable, not a type
}

