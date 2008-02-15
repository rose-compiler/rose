// t0020.c
// ansi C coverage

int     //ERRORIFMISSING(1): required
foo()
{
  return 2;
}

void char1()
{
  int ch;

  // illegal because wchar_t not defined yet
  //ERROR(2): ch = L'x';
}

typedef int wchar_t;

void char2()
{
  int ch;

  ch = L'x';   // ok
}


// EOF
