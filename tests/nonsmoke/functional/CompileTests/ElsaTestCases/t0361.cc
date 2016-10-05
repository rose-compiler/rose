// t0361.cc
// declare member function with qualified name

struct C {
  int x;

  // icc says this syntax is illegal                
  //
  // gcc says it is ok
  //
  // right now, Elsa rejects it
  int 
    //ERROR(2): C::
    foo();
  
  // related: actually, Elsa is failing an assertion right now
  // whenever a static method uses a nonstatic variable (now fixed)
  static int bar();
};

int C::foo()
{
  return x;
}

int C::bar()
{
  int ret = 1;
  //ERROR(1): ret += x;
  return ret;
}

