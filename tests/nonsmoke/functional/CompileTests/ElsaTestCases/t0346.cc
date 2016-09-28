// t0346.cc
// type of character literals

void foo()
{
  __checkType('a', (char)0);
  __checkType('ab', (int)0);
  __checkType(L'a', (wchar_t)0);
}

// EOF
