// cc.in74
// need global operator new defined

int foo()
{
  return operator new(3);
}

int foo2()
{
  return ::operator new(4);
}
