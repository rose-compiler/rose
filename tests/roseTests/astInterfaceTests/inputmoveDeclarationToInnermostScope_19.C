void func()
{
  int i;

  {
    { i = 0; }
    { i = 0; }
  }

  {
     i = 0;
  }
}
