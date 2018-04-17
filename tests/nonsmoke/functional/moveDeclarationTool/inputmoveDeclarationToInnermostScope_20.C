void func()
{
  int i;

  {
      for (i = 0; ; ) {}
      for (i = 0; ; ) {}
  }

  for (i = 0; ; ) {
  }
}
