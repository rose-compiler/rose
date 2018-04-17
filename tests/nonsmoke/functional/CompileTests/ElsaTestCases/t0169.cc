// t0169.cc
// problem with scope of 'if' substatements?

void f()
{       
  // are all three different decls, and therefore allowed?
  // yes: cppstd 6.4 para 1

  if (true)
    int x;
  else
    int x;
  int x;
}


// test switch statement as well (other part of 6.4 para 1)
void g()
{
  switch (4)
    int x;
  int x;
}


// and 6.5 para 2
void h()
{
  while (false)
    int x;      

  do
    int x;
  while (false);
  
  for ( ; false; )
    int x;
    
  int x;
}
