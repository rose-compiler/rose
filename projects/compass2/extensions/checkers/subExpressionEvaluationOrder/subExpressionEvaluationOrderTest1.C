
//Your test file code goes here.

int bar(int a, int b);

void foo()
{
  int i = 0;

  i = bar(++i, ++i); // either ++i could be evaluated first
  i = bar(2, 3); // fine
  i = bar((i=3), (i=4)); // no particular order is guaranteed.
  i = bar((i=2), 3); //fine
}

