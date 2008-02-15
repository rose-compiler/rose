// t0014.c
// type of character literals

typedef long wchar_t;

void foo()
{
  __checkType('a', (int)0);
  __checkType('ab', (int)0);
  __checkType(L'a', (wchar_t)0);
}
