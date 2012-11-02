int global;
int global2=3;
void setMe()
{
  global=1;
  global2=3;
}

int foo()
{
  int a;
  setMe();
  a=global;
  global2=0;
  return 0;
}
