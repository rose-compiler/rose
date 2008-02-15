// cc.in6
// one example of each statement (no ambiguity)

int something, a, whatever, something_else, x;
typedef int X;

int main()
{
  3+4;
mylabel:
  switch (something) {
    case 5:
      { 6; 7; }
      break;

    default:
      ;
  }

  if (a) 8; else 9;
  if (a) 10;

  // precedence should make this unambiguous too
  {
    if (a) 11;
    if (a) 12; else 13;
  }

  while (whatever) 14;

  // variant which declares a variable in the condition
  while (bool cond = whatever) 15;

  do 16; while (something_else);

  // no decl in the initializer
  int i;
  for (i=0; i<4; i++) 17;

  // decl in init
  for (int j=9; j>0; j--) 18;

  // both variants of 'return'
  if (a) return; else return x;

  goto mylabel;

  try {
    19;
  }
  catch (X &x) {
    throw 20;
  }
  catch (...) {
    21;
    throw;
  }

  4;   // dummy for 'try' CFG targets
}
