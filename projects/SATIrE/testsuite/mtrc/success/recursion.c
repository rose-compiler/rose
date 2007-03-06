/* $Id: recursion.c,v 1.1 2008-04-25 10:33:55 adrian Exp $ */

/* Generate an example of recursive code, to see  *
 * how it can be modeled in the scope graph.      */

/* self-recursion  */
int fib(int i)
{
  if(i==0)
    return 1;
  if(i==1)
    return 1;
  return fib(i-1) + fib(i-2);
}

/* mutual recursion */
int anka(int);

int kalle(int i)
{
  if(i<=0)
    return 0;
  else
    return anka(i-1);
}

int anka(int i)
{
  if(i<=0)
    return 1;
  else
    return kalle(i-1);
}

extern volatile int In;

void main(void)
{
  In = fib(10);
}
