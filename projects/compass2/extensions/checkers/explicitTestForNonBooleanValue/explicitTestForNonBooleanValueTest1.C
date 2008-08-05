
//Your test file code goes here.

int bar();

void foo()
{
  int i;
  if(bar())
    i = 2;

  while(bar())
    i = 3;

  do {
    i = 4;
  } while(bar());

  for(i=0; bar(); i++)
    i =5;

  i = (bar() ? 6 : 7);

  for(i = (bar() ? 8 : 9); bar(); i++)
    i = 10;

  if(0 != bar())
    i = 11;
}

